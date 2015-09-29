#include "PaxosLease.h"
#include "PLeaseConsts.h"
#include "ReplicatedLog.h"
#include "common.h"

MINA_NAMESPACE_START
PaxosLease::PaxosLease()
{
}

void PaxosLease::Init(CSession* apoSession)
{
    mpoSession = apoSession;
	proposer.Init(mpoSession);
	acceptor.Init(mpoSession);
	learner.Init(mpoSession);
	learner.SetLeaser(this);

	acquireLease = false;
}

void PaxosLease::Shutdown()
{

}

void PaxosLease::InitTransport()
{
}

void PaxosLease::OnRead()
{
    mina::CMemblock *bs;
	bs = mpoSession->GetRead()->GetBlock();
	if (!msg.Read(*bs))
		return;

    msg.Dump();

	if ((msg.IsRequest()) && msg.proposalID > proposer.highestProposalID)
    {
        proposer.highestProposalID = msg.proposalID;
    }

	if (msg.IsResponse())
		proposer.ProcessMsg(msg);
	else if (msg.IsRequest())
		acceptor.ProcessMsg(msg);
	else if (msg.type == PLEASE_LEARN_CHOSEN)
		learner.ProcessMsg(msg);
	else
		ASSERT_FAIL();
}

void PaxosLease::AcquireLease()
{
	acquireLease = true;
	proposer.StartAcquireLease();
}

bool PaxosLease::IsLeaseOwner()
{
	return learner.IsLeaseOwner();
}

bool PaxosLease::IsLeaseKnown()
{
	return learner.IsLeaseKnown();
}

unsigned PaxosLease::GetLeaseOwner()
{
	return learner.GetLeaseOwner();
}

uint64_t PaxosLease::GetLeaseEpoch()
{
	return learner.GetLeaseEpoch();
}

void PaxosLease::SetLog(ReplicatedLog* apoLog)
{
	mpoLog = apoLog;
}


void PaxosLease::Stop()
{
}

void PaxosLease::Continue()
{
}

bool PaxosLease::IsActive()
{
	return mpoSession->ReadActive();
}

void PaxosLease::OnNewPaxosRound()
{
	proposer.OnNewPaxosRound();
}

void PaxosLease::OnLearnLease()
{
    mpoLog->OnLearnLease();
	if (!IsLeaseOwner())
		proposer.StopAcquireLease();
}

void PaxosLease::OnLeaseTimeout()
{
	mpoLog->OnLeaseTimeout();
	if (acquireLease)
		proposer.StartAcquireLease();
}

void PaxosLease::OnStartupTimeout()
{
}
MINA_NAMESPACE_END
