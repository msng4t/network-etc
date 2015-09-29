#include "PLeaseProposer.h"
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "ReplicatedLog.h"
#include "ReplicatedConfig.h"
#include "PLeaseConsts.h"
#include "IoService.h"
#include "Cycle.h"
#include "Timer.h"

MINA_NAMESPACE_START
#define Now  ((uint64_t)mpoSession->GetService()->GetCycle()->GetTimer()->GetSec())

PLeaseProposer::PLeaseProposer()
{
    wdata.Init(1024);
}

void PLeaseProposer::Init(mina::CSession* writers_)
{
	mpoSession = writers_;
	highestProposalID = 0;
	state.Init();
    mpoTimer = mpoSession->GetCycle()->GetTimer();
    mpoAcqTimer = new CIoEvent;
    mpoExtTimer = new CIoEvent;
    mpoAcqTimer->SetSession(mpoSession);
    mpoExtTimer->SetSession(mpoSession);
    mpoAcqTimer->SetLock(&muAcqLock);
    mpoAcqTimer->SetOwnLock(&muAcqLock);
    mpoExtTimer->SetLock(&muExtLock);
    mpoExtTimer->SetOwnLock(&muExtLock);
    mpoAcqTimer->SetTimerHandler(new AcquireHandler(this));
    mpoExtTimer->SetTimerHandler(new ExtendedHandler(this));
    LOG_TRACE("Proposer's Acq:%p, Ext:%p", mpoAcqTimer, mpoExtTimer);
}

void PLeaseProposer::StartAcquireLease()
{
	mpoTimer->Delete(mpoExtTimer);

	if (!(state.preparing || state.proposing))
		StartPreparing();
}

void PLeaseProposer::StopAcquireLease()
{

	state.preparing = false;
	state.proposing = false;
	mpoTimer->Delete(mpoExtTimer);
	mpoTimer->Delete(mpoAcqTimer);
}

void PLeaseProposer::OnNewPaxosRound()
{
	// since PaxosLease msgs carry the paxosID, and nodes
	// won't reply if their paxosID is larger than the msg's
	// if the paxosID increases we must restart the
	// PaxosLease round, if it's active
	// only restart if we're masters

	LOG_TRACES();
	//
	//if (acquireLeaseTimeout.IsActive() && RLOG->IsMaster())
	//	StartPreparing();
}

void PLeaseProposer::BroadcastMessage()
{
    CSockAddr loAddr;
	LOG_TRACES();

	numReceived = 0;
	numAccepted = 0;
	numRejected = 0;

    wdata.Reset();
    msg.Dump();
	msg.Write(wdata);

    if (RCONF->GetMultiMode())
    {
        LOG_TRACE("MultiCastMode Send");
        mpoSession->GetDest()->Set("224.0.0.88", 8888);
        mpoSession->Write(&wdata);
    }
    else
    {
        for (size_t nodeID = 0; nodeID < RCONF->GetNumNodes(); nodeID++)
        {
//            if (nodeID == RCONF->GetNodeID())
//            {
//                continue;
//            }
            LOG_TRACE("Node %d %s:%d", nodeID, RCONF->GetAddr(nodeID).GetIP(),RCONF->GetAddr(nodeID).GetPort());
            mpoSession->GetDest()->Set(RCONF->GetAddr(nodeID));
            mpoSession->Write(&wdata);
        }
    }
}

void PLeaseProposer::ProcessMsg(PLeaseMsg &msg_)
{
	msg = msg_;

	if (msg.IsPrepareResponse())
		OnPrepareResponse();
	else if (msg.IsProposeResponse())
		OnProposeResponse();
	else
		ASSERT_FAIL();
}

void PLeaseProposer::OnPrepareResponse()
{
	LOG_TRACES();

	if (!state.preparing || msg.proposalID != state.proposalID)
		return;

	numReceived++;

	if (msg.type == PLEASE_PREPARE_REJECTED)
		numRejected++;
	else if (msg.type == PLEASE_PREPARE_PREVIOUSLY_ACCEPTED &&
			 msg.acceptedProposalID >= state.highestReceivedProposalID)
	{
		state.highestReceivedProposalID = msg.acceptedProposalID;
		state.leaseOwner = msg.leaseOwner;
	}

	if (numRejected >= ceil((double)(RCONF->GetNumNodes()) / 2)) // (x/2 + 1) also works
	{
		StartPreparing();
		return;
	}

	// see if we have enough positive replies to advance	
	if ((numReceived - numRejected) >= RCONF->MinMajority())
		StartProposing();	
}

void PLeaseProposer::OnProposeResponse()
{
	LOG_TRACES();

	if (state.expireTime < Now)
	{
		LOG_TRACE("already expired, wait for timer");
		return; // already expired, wait for timer
	}

	if (!state.proposing || msg.proposalID != state.proposalID)
	{
		LOG_TRACE("not my proposal");
		return;
	}

	numReceived++;

	if (msg.type == PLEASE_PROPOSE_ACCEPTED)
		numAccepted++;

	LOG_TRACE("numAccepted: %d", numAccepted);

	// see if we have enough positive replies to advance
	if (numAccepted >= RCONF->MinMajority() &&
	state.expireTime - Now > 500 /*msec*/)
	{
		// a majority have accepted our proposal, we have consensus
		state.proposing = false;
		msg.LearnChosen(RCONF->GetNodeID(),
		state.leaseOwner, state.expireTime - Now, state.expireTime);

		mpoTimer->Delete(mpoAcqTimer);

//		extendLeaseTimeout.Set(Now + (state.expireTime - Now) / 7);
		mpoTimer->Add(mpoExtTimer, (state.expireTime - Now) / 7);

		BroadcastMessage();
		return;
	}

	if (numReceived == RCONF->GetNumNodes())
		StartPreparing();
}

void PLeaseProposer::StartPreparing()
{
    LOG_TRACE("StartPreparing");

	mpoTimer->Add(mpoAcqTimer, ACQUIRELEASE_TIMEOUT);

	state.proposing = false;

	state.preparing = true;

	state.leaseOwner = RCONF->GetNodeID();

	state.highestReceivedProposalID = 0;

	state.proposalID = RCONF->NextHighest(highestProposalID);

	if (state.proposalID > highestProposalID)
		highestProposalID = state.proposalID;

	msg.PrepareRequest(RCONF->GetNodeID(),
	state.proposalID, RLOG->GetPaxosID());

	BroadcastMessage();
}

void PLeaseProposer::StartProposing()
{
	LOG_TRACES();

	state.preparing = false;

	if (state.leaseOwner != RCONF->GetNodeID())
		return; // no point in getting someone else a lease,
				// wait for OnAcquireLeaseTimeout

	state.proposing = true;

	state.duration = MAX_LEASE_TIME;
	state.expireTime = Now + state.duration;
	msg.ProposeRequest(RCONF->GetNodeID(), state.proposalID,
		state.leaseOwner, state.duration);

	BroadcastMessage();
}

void PLeaseProposer::OnAcquireLeaseTimeout()
{
	LOG_TRACES();

	StartPreparing();
}

void PLeaseProposer::OnExtendLeaseTimeout()
{
	LOG_TRACES();

	assert(!(state.preparing || state.proposing));

	StartPreparing();
}
MINA_NAMESPACE_END
