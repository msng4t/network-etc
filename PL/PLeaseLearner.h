#ifndef PLEASELEARNER_H
#define PLEASELEARNER_H

#include "PLeaseMsg.h"
#include "PLeaseState.h"
#include "Session.h"

MINA_NAMESPACE_START
class PaxosLease;
class PLeaseLearner
{
	typedef PLeaseLearnerState		State;
	class CLeaseHandler;
public:
	PLeaseLearner();

	void		Init(CSession* apoSession);

	void		ProcessMsg(PLeaseMsg &msg);
	void		OnLeaseTimeout();
	bool		IsLeaseOwner();
	bool		IsLeaseKnown();
	int			GetLeaseOwner();
	uint64_t	GetLeaseEpoch();
    void        SetLeaser(PaxosLease* apoLeaser);

private:
	void		OnLearnChosen();
	void		CheckLease();

	PLeaseMsg	msg;
	State		state;
	bool		useSoftClock;
    PaxosLease* mpoLeaser;
    CSession* mpoSession;
    CIoEvent* mpoLeaseTimer;
    volatile uint64_t muLock;


    class CLeaseHandler : public CTimeoutHandler
    {
        public:
            CLeaseHandler(PLeaseLearner* apoLearner)
            {
                mpoLearner = apoLearner;
            }
            int HandleTimeout( void )
            {
                mpoLearner->OnLeaseTimeout();
                return 0;
            }
        private:
            PLeaseLearner* mpoLearner;
    };
};

MINA_NAMESPACE_END
#endif
