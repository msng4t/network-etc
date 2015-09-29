#ifndef PLEASEACCEPTOR_H
#define PLEASEACCEPTOR_H

#include "TimeoutHandler.h"
#include "PLeaseConsts.h"
#include "PLeaseMsg.h"
#include "PLeaseState.h"
#include "Session.h"

MINA_NAMESPACE_START
class ReplicatedLog;

class PLeaseAcceptor
{
class CLeaseHandler;
typedef PLeaseAcceptorState State;
public:
	PLeaseAcceptor();

	void		Init(mina::CSession* apoSession);
	void		ProcessMsg(PLeaseMsg &msg_);
	void		OnLeaseTimeout();
    void        HandleTimeout(void){this->OnLeaseTimeout();}

private:
	void		SendReply(unsigned nodeID);

	void		OnPrepareRequest();
	void		OnProposeRequest();

    CMemblock wdata;
    CSession *mpoSession;
    CIoEvent *mpoLeaseTimer;
    volatile uint64_t muLock;

	PLeaseMsg	msg;
	State		state;

    class CLeaseHandler: public CTimeoutHandler
    {
        public:
            CLeaseHandler(PLeaseAcceptor* apoAcceptor)
            {
                mpoAcceptor = apoAcceptor;
            }
            int HandleTimeout( void )
            {
                mpoAcceptor->OnLeaseTimeout();
                return 0;
            }
        private:
            PLeaseAcceptor* mpoAcceptor;
    };
};


MINA_NAMESPACE_END
#endif
