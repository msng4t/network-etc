#ifndef PAXOSLEASE_H
#define PAXOSLEASE_H

#include "ReplicatedConfig.h"
#include "PLeaseConsts.h"
#include "PLeaseMsg.h"
#include "PLeaseProposer.h"
#include "PLeaseAcceptor.h"
#include "PLeaseLearner.h"
#include "Session.h"

MINA_NAMESPACE_START
class ReplicatedLog;

class PaxosLease
{
public:
	PaxosLease();

	void			Init(CSession* apoSession);
	void			Shutdown();
	void			OnRead();
	void			AcquireLease();
	bool			IsLeaseOwner();
	bool			IsLeaseKnown();
	unsigned		GetLeaseOwner();
	uint64_t		GetLeaseEpoch();
	void			SetLog(ReplicatedLog* apoLog);
	void			Stop();
	void			Continue();
	bool			IsActive();
	void			OnNewPaxosRound();
	void			OnLearnLease();
	void			OnLeaseTimeout();
	void			OnStartupTimeout();
	
private:
	void			InitTransport();
	
	bool			acquireLease;
	PLeaseMsg		msg;
 	PLeaseProposer	proposer;
	PLeaseAcceptor	acceptor;
	PLeaseLearner	learner;
    CSession* mpoSession;

    ReplicatedLog* mpoLog;
};

MINA_NAMESPACE_END
#endif
