#ifndef PLEASEPROPOSER_H
#define PLEASEPROPOSER_H

#include "common.h"
#include "Session.h"
#include "PaxosState.h"
#include "PLeaseConsts.h"
#include "PLeaseMsg.h"
#include "PLeaseState.h"

MINA_NAMESPACE_START
class ReplicatedLog;
class AcquireHandler;
class ExtendedHandler;
class PLeaseProposer
{
typedef PLeaseProposerState State;
public:
	PLeaseProposer();

	void		Init(mina::CSession *writers_);

	void		ProcessMsg(PLeaseMsg &msg_);
	void		OnNewPaxosRound();
	void		OnAcquireLeaseTimeout();
	void		OnExtendLeaseTimeout();
	void		StartAcquireLease();
	void		StopAcquireLease();

	uint64_t	highestProposalID;

private:
	void		BroadcastMessage();
	void		OnPrepareResponse();
	void		OnProposeResponse();
	void		StartPreparing();
	void		StartProposing();

    CSession *mpoSession;
    CMemblock wdata;
    CTimer*     mpoTimer;
	State		state;
	PLeaseMsg	msg;
// keeping track of messages during prepare and propose phases
	unsigned	numReceived;
	unsigned	numAccepted;
	unsigned	numRejected;
    CIoEvent* mpoAcqTimer;
    volatile uint64_t muAcqLock;

    CIoEvent* mpoExtTimer;
    volatile uint64_t muExtLock;

};
class ExtendedHandler: public CTimeoutHandler
{
    public:
        ExtendedHandler(PLeaseProposer* apoProposer)
        {
            mpoProposer = apoProposer;
        }
        int HandleTimeout( void )
        {
            mpoProposer->OnExtendLeaseTimeout();
            return 0;
        }
    private:
        PLeaseProposer* mpoProposer;
};
class AcquireHandler: public CTimeoutHandler
{
    public:
        AcquireHandler(PLeaseProposer* apoProposer)
        {
            mpoProposer = apoProposer;
        }
        int HandleTimeout( void )
        {
            mpoProposer->OnAcquireLeaseTimeout();
            return 0;
        }
    private:
        PLeaseProposer* mpoProposer;
};

MINA_NAMESPACE_END
#endif
