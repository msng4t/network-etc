#include "PLeaseAcceptor.h"
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>
#include "ReplicatedLog.h"
#include "ReplicatedConfig.h"
#include "PLeaseConsts.h"
#include "IoService.h"
#include "Cycle.h"
#include "Timer.h"

MINA_NAMESPACE_START
PLeaseAcceptor::PLeaseAcceptor()
    : muLock (0)
{
    wdata.Init(1024);
}

void PLeaseAcceptor::Init(mina::CSession* apoSession)
{
    mpoSession = apoSession;

	state.Init();
    mpoLeaseTimer = new CIoEvent;
    mpoLeaseTimer->SetSession(mpoSession);
    mpoLeaseTimer->SetLock(&muLock);
    mpoLeaseTimer->SetOwnLock(&muLock);
    mpoLeaseTimer->SetTimerHandler(new CLeaseHandler(this));
    LOG_TRACE("Accepter's :%p", mpoLeaseTimer);
}

void PLeaseAcceptor::SendReply(unsigned nodeID)
{
    wdata.Reset();
    msg.Dump();
	msg.Write(wdata);

    LOG_TRACE("send %d:%s to Node %d %s:%d", wdata.GetSize(), wdata.GetRead(),
            nodeID, RCONF->GetAddr(nodeID).GetIP(),RCONF->GetAddr(nodeID).GetPort());
    mpoSession->GetDest()->Set(RCONF->GetAddr(nodeID));
    mpoSession->Write(&wdata);
}

void PLeaseAcceptor::ProcessMsg(PLeaseMsg& msg_)
{
	msg = msg_;

	LOG_TRACE("ProcessMsg msg.paxosID: %llu, msg_: %llu",
	msg.paxosID, msg_.paxosID);
   assert (msg.paxosID == msg_.paxosID);
	if (msg.type == PLEASE_PREPARE_REQUEST)
		OnPrepareRequest();
	else if (msg.type == PLEASE_PROPOSE_REQUEST)
		OnProposeRequest();
	else
		ASSERT_FAIL();
}

void PLeaseAcceptor::OnPrepareRequest()
{
	LOG_TRACE("OnPrepareRequest msg.paxosID: %llu, my.paxosID: %llu",
	msg.paxosID, RLOG->GetPaxosID());

	if (msg.paxosID < RLOG->GetPaxosID() && (int) msg.nodeID != RLOG->GetMaster())
		return; // only up-to-date nodes can become masters

	RLOG->OnPaxosLeaseMsg(msg.paxosID, msg.nodeID); //now null

	unsigned senderID = msg.nodeID;

	if (state.accepted && state.acceptedExpireTime <
            (uint64_t)mpoSession->GetService()->GetCycle()->GetTimer()->GetSec())
	{
	    LOG_TRACE("OnLeaseTimeout");
		OnLeaseTimeout();
	}

	if (msg.proposalID < state.promisedProposalID)
		msg.PrepareRejected(RCONF->GetNodeID(), msg.proposalID);
	else
	{
		state.promisedProposalID = msg.proposalID;

		if (!state.accepted)
			msg.PrepareCurrentlyOpen(RCONF->GetNodeID(),
			msg.proposalID);
		else
			msg.PreparePreviouslyAccepted(RCONF->GetNodeID(),
			msg.proposalID, state.acceptedProposalID,
			state.acceptedLeaseOwner, state.acceptedDuration);
	}

    LOG_TRACE("SendReply in OnPrepareRequest %u", senderID);
	SendReply(senderID);
}

void PLeaseAcceptor::OnProposeRequest()
{
	unsigned senderID = msg.nodeID;

    LOG_TRACE("OnProposeRequest msg.paxosID: %llu, my.paxosID: %llu",
            msg.paxosID, RLOG->GetPaxosID());

	if (state.accepted && state.acceptedExpireTime <
            (uint64_t)mpoSession->GetService()->GetCycle()->GetTimer()->GetSec())
	{
		OnLeaseTimeout();
	}

	if (msg.proposalID < state.promisedProposalID)
		msg.ProposeRejected(RCONF->GetNodeID(), msg.proposalID);
	else
	{
		state.accepted = true;
		state.acceptedProposalID = msg.proposalID;
		state.acceptedLeaseOwner = msg.leaseOwner;
		state.acceptedDuration = msg.duration;
        state.acceptedExpireTime = state.acceptedDuration
            + mpoSession->GetService()->GetCycle()->GetTimer()->GetSec();
        mpoSession->GetService()->GetCycle()->GetTimer()->Add(mpoLeaseTimer, state.acceptedDuration);

		msg.ProposeAccepted(RCONF->GetNodeID(), msg.proposalID);
	}

    LOG_TRACE("SendReply in OnProposeRequest %u", senderID);
	SendReply(senderID);
}

void PLeaseAcceptor::OnLeaseTimeout()
{
	state.OnLeaseTimeout();
}

MINA_NAMESPACE_END
