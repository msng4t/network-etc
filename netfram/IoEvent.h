#ifndef _IO_EVENT_INCLUDE_
#define _IO_EVENT_INCLUDE_
#include "Define.h"
#include "RBTree.h"
#include "Memblock.h"
#include "TimeValue.h"
#include "SharedMutex.h"
#include "Queue.h"
#include "Log.h"
#include "Singleton.h"
#include "TimeoutHandler.h"
#include "TSKipList.h"

#define GENGETSET(var) \
    inline volatile bool& var(void)  { return mb##var;}\
    void inline var(volatile bool value) { this->mb##var = value;}

MINA_NAMESPACE_START
class CSession;

#ifndef SKIPTIMER
class CIoEvent :public CRBTree<uint64_t>::Node, public CQueueNode
#else
class CIoEvent :public CSkipList<uint64_t, CIoEvent*>::Node, public CQueueNode
#endif
{
public:
    ~CIoEvent();
    CIoEvent(void);
    CSession* GetSession(void);
    void SetSession(CSession *apoSession);
    bool Lock(void);
    void Reset(void);
    void SetLock(volatile uint64_t *apuLock);
    volatile uint64_t* GetLock(void) volatile;
    volatile uint64_t* GetOwnLock(void) volatile;
    void Unlock(void);
    void SetOwnLock(volatile uint64_t *apuLock);
    bool LockOwn(void);
    void UnlockOwn(void);
    bool LockEqualOwn(void);

    int HandleInput(uint32_t auFlags);

    int HandleOutput(uint32_t auFlags);

    int HandleException( void );

    int HandleTimeout( void );

    void SetTimerHandler(CTimeoutHandler* apoHandler);

    int PostTimeout( CTimeValue const& aoCurrentTime);

    int HandleClose( int aiMask );

    CMemblock* GetBlock(void);

    void SetBlock(CMemblock* apoBlock);

    GENGETSET(Timeout);
    GENGETSET(TimerOn);
    GENGETSET(Accept);
    GENGETSET(Active);
    GENGETSET(Ready);
    GENGETSET(Channel);
    GENGETSET(Write);
    GENGETSET(Locked);
    GENGETSET(Closed);
    GENGETSET(Extended);
private:
    CSession* mpoSession;
    volatile uint64_t *mpuLock;
    volatile uint64_t *mpuOwnLock;
    volatile bool mbTimeout;
    volatile bool mbTimerOn;
    volatile bool mbWrite;
    volatile bool mbAccept;
    volatile bool mbChannel;
    volatile bool mbActive;
    volatile bool mbReady;
    volatile bool mbClosed;
    volatile bool mbLocked;
    volatile bool mbExtended;

    CMemblock* mpoBlock;

};


class CIoTimer: public CTimeoutHandler
{
public:
    CIoTimer(CIoEvent* apoIoEvent)
    {
        mpoIoEvent = apoIoEvent;
    }
    int HandleTimeout( void );
private:
    CIoEvent* mpoIoEvent;
};

class CTimerEvent: public CTimeoutHandler
{
public:
    CTimerEvent()
    {
        mpoIoEvent = new CIoEvent;
        mpoIoEvent->SetLock(&muLock);
        mpoIoEvent->SetOwnLock(&muLock);
        mpoIoEvent->SetTimerHandler(this);
        muLock = 0;
    }
    int HandleTimeout( void )
    {
      LOG_DEBUG("_Ter.\n");
      return 0;
    }
    CIoEvent* GetEvent()
    {
      return mpoIoEvent;
    }
    ~CTimerEvent()
    {
      delete mpoIoEvent;
    }
private:
    CIoEvent* mpoIoEvent;
    volatile uint64_t muLock;
};

MINA_NAMESPACE_END
#endif
