#ifndef REPLICATEDLOG_H
#define REPLICATEDLOG_H

#include "PaxosLease.h"
#include "PLeaseAcceptor.h"
#include "PLeaseLearner.h"
#include "PLeaseProposer.h"
#include "../BDB/Transaction.h"
#include "Session.h"

#define CATCHUP_TIMEOUT	5000

#define	RLOG (ReplicatedLog::Get())

MINA_NAMESPACE_START
class ReplicatedLog
{
public:

	static ReplicatedLog* Get();

	bool				Init(CSession* apoSession);
	void				Shutdown();

	bool				IsMaster();
	int					GetMaster();
	unsigned			GetNumNodes();
	unsigned 			GetNodeID();
	void				StopMasterLease();
	void				ContinueMasterLease();
	bool				IsMasterLeaseActive();
	void				OnPaxosLeaseMsg(uint64_t paxosID, unsigned nodeID);
	void				OnLearnLease();
	void				OnLeaseTimeout();
    bool                IsPaxosActive();
    uint64_t GetPaxosID();
    void SetPaxosID(CTransaction* transaction, uint64_t paxosID);
    PaxosLease          *GetLeaser( void );

private:
	ReplicatedLog();


	PaxosLease			masterLease;
	uint64_t			highestPaxosID;
    mina::CSession*     mpoSession;

	uint64_t			lastStarted;
	uint64_t			lastLength;
	uint64_t			lastTook;
	uint64_t			thruput;
};
MINA_NAMESPACE_END
#endif
