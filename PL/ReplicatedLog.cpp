#include "ReplicatedLog.h"
#include <string.h>
#include <stdlib.h>
#include "IoService.h"
#include "Cycle.h"
#include "Timer.h"

MINA_NAMESPACE_START
#define Now  ((uint64_t)mpoSession->GetService()->GetCycle()->GetTimer()->GetSec())

ReplicatedLog* replicatedLog = NULL;

ReplicatedLog* ReplicatedLog::Get()
{
	if (replicatedLog == NULL)
		replicatedLog = new ReplicatedLog;
	
	return replicatedLog;
}

ReplicatedLog::ReplicatedLog()
{
}

bool ReplicatedLog::Init(CSession* apoSession)
{
	highestPaxosID = 0;
    mpoSession = apoSession;
	lastStarted = Now;
	lastLength = 0;
	lastTook = 0;
	thruput = 0;

	masterLease.Init(apoSession);
	masterLease.SetLog(this);
	masterLease.AcquireLease();

	return true;
}

void ReplicatedLog::Shutdown()
{
	masterLease.Shutdown();

	replicatedLog = NULL;
	delete this;
}

void ReplicatedLog::StopMasterLease()
{
	masterLease.Stop();
}

void ReplicatedLog::ContinueMasterLease()
{
	masterLease.Continue();
}

bool ReplicatedLog::IsPaxosActive()
{
	return mpoSession->ReadActive();
}

bool ReplicatedLog::IsMasterLeaseActive()
{
	return masterLease.IsActive();
}


bool ReplicatedLog::IsMaster()
{
	return masterLease.IsLeaseOwner();
}

int ReplicatedLog::GetMaster()
{
	return masterLease.GetLeaseOwner();
}

uint64_t ReplicatedLog::GetPaxosID()
{
    return RCONF->GetNodeID();
}

void ReplicatedLog::SetPaxosID(CTransaction* transaction, uint64_t paxosID)
{
}

unsigned ReplicatedLog::GetNumNodes()
{
	return RCONF->GetNumNodes();
}

unsigned ReplicatedLog::GetNodeID()
{
	return RCONF->GetNodeID();
}

void ReplicatedLog::OnLearnLease()
{
	if (masterLease.IsLeaseOwner() )
	{

		LOG_TRACE("appending NOP to assure safeDB");
	}
}

void ReplicatedLog::OnLeaseTimeout()
{

//    replicatedDB->OnMasterLeaseExpired();
}

void ReplicatedLog::OnPaxosLeaseMsg(uint64_t paxosID, unsigned nodeID)
{
}

PaxosLease* ReplicatedLog::GetLeaser( void )
{
    return &masterLease;
}
MINA_NAMESPACE_END
