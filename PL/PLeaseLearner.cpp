#include <inttypes.h>
#include "PLeaseLearner.h"
#include "PLeaseConsts.h"
#include "IoService.h"
#include "Cycle.h"
#include "Timer.h"
#include "ReplicatedLog.h"

MINA_NAMESPACE_START
#define Now  ((uint64_t)mpoSession->GetService()->GetCycle()->GetTimer()->GetSec())

PLeaseLearner::PLeaseLearner()
    : muLock (0)
{
}

void PLeaseLearner::Init(CSession* apoSession)
{
	state.Init();
	useSoftClock = false;
    this->mpoSession = apoSession;

    mpoLeaseTimer = new CIoEvent;
    mpoLeaseTimer->SetSession(mpoSession);
    mpoLeaseTimer->SetLock(&muLock);
    mpoLeaseTimer->SetOwnLock(&muLock);
    mpoLeaseTimer->SetTimerHandler(new CLeaseHandler(this));
    LOG_TRACE("Accepter's :%p", mpoLeaseTimer);
}

void PLeaseLearner::ProcessMsg(PLeaseMsg &msg_)
{
	msg = msg_;

	if (msg.type == PLEASE_LEARN_CHOSEN)
		OnLearnChosen();
	else
		ASSERT_FAIL();
}

void PLeaseLearner::OnLearnChosen()
{

    msg.Dump();
	if (state.learned && state.expireTime < Now)
		OnLeaseTimeout();

	uint64_t expireTime;
	if (msg.leaseOwner == RCONF->GetNodeID())
		expireTime = msg.localExpireTime; // I'm the master
	else
		expireTime = Now + msg.duration - 500 /* msec */;
		// conservative estimate

	if (expireTime < Now)
		return;

	if (!state.learned && RLOG->IsMasterLeaseActive())
		LOG_TRACE("Node %d is the master", msg.leaseOwner);

	state.learned = true;
	state.leaseOwner = msg.leaseOwner;
	state.expireTime = expireTime;
	LOG_TRACE("+++ Node %d has the lease for %llu msec +++",
		state.leaseOwner, state.expireTime - Now);

    mpoSession->GetService()->GetCycle()->GetTimer()->Add(mpoLeaseTimer, state.expireTime);

	mpoLeaser->OnLearnLease();
}

void PLeaseLearner::OnLeaseTimeout()
{
	if (state.learned && RLOG->IsMasterLeaseActive())
		LOG_TRACE("Node %d is no longer the master",
		state.leaseOwner);

    mpoSession->GetService()->GetCycle()->GetTimer()->Delete(mpoLeaseTimer);

	state.OnLeaseTimeout();

	mpoLeaser->OnLeaseTimeout();
}

void PLeaseLearner::CheckLease()
{
	uint64_t now;
	
	if (useSoftClock)
		now = Now;
	else
		now = Now;
	
	if (state.learned && state.expireTime < now)
		OnLeaseTimeout();
}

bool PLeaseLearner::IsLeaseOwner()
{
	CheckLease();
	
	if (state.learned && state.leaseOwner == RCONF->GetNodeID())
		return true;
	else
		return false;
}

bool PLeaseLearner::IsLeaseKnown()
{
	CheckLease();
	
	if (state.learned)
		return true;
	else
		return false;	
}

int PLeaseLearner::GetLeaseOwner()
{
	CheckLease();
	
	if (state.learned)
		return state.leaseOwner;
	else
		return -1;
}

uint64_t PLeaseLearner::GetLeaseEpoch()
{
	CheckLease();
	
	return state.leaseEpoch;
}

void PLeaseLearner::SetLeaser(PaxosLease* apoLeaser)
{
	mpoLeaser = apoLeaser;
}
MINA_NAMESPACE_END
