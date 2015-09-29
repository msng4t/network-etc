#include "IoEvent.h"
#include "Cycle.h"
MINA_NAMESPACE_START

CIoEvent::CIoEvent(void)
    : mpoSession (NULL)
    , mpuLock (NULL)
    , mpuOwnLock (NULL)
    , mbTimeout (false)
    , mbTimerOn(false)
    , mbWrite (false)
    , mbAccept (false)
    , mbActive (false)
    , mbReady (false)
    , mpoTimerHandler (NULL)
    , mpoBlock (NULL)
{
}

void CIoEvent::Reset(void)
{
    mpoSession = NULL;
    mpuLock = NULL;
    mpuOwnLock = NULL;
    mbTimeout = false;
    mbTimerOn = false;
    mbWrite = false;
    mbAccept = false;
    mbActive = false;
    mbReady = false;
    if (mpoBlock)
      mpoBlock->Reset();
    Init();
}


CIoEvent::~CIoEvent()
{
    if (mpoBlock)
      delete mpoBlock;
    if (mpoTimerHandler)
      delete mpoTimerHandler;
}

CSession*
CIoEvent::GetSession(void)
{
    return this->mpoSession;
}

CMemblock* CIoEvent::GetBlock(void)
{
    if (mpoBlock == NULL)
    {
        this->mpoBlock = new CMemblock(1024);
    }
    else if (mpoBlock->GetCapacity() == 0)
    {
        this->mpoBlock->Resize(1024);
    }
    return this->mpoBlock;
}

void CIoEvent::SetBlock(CMemblock* apoBlock)
{
    this->mpoBlock = apoBlock;
}

volatile uint64_t* CIoEvent::GetLock(void) volatile
{
    return this->mpuLock;
}
volatile uint64_t* CIoEvent::GetOwnLock(void) volatile
{
    return this->mpuOwnLock;
}

void CIoEvent::SetSession(CSession *apoSession)
{
    this->mpoSession = apoSession;
}

void CIoEvent::SetLock(volatile uint64_t *apuLock)
{
    this->mpuLock = apuLock;
}

void CIoEvent::SetOwnLock(volatile uint64_t *apuLock)
{
    this->mpuOwnLock = apuLock;
}

bool CIoEvent::Lock(void)
{
    return *(this->mpuLock) == 0 && MinaAtomicCmpSet(mpuLock, 0, 1);
}

void CIoEvent::Unlock(void)
{
    *(this->mpuLock) = 0;
}
bool CIoEvent::LockOwn(void)
{
    return *(this->mpuOwnLock) == 0 && MinaAtomicCmpSet(mpuOwnLock, 0, 1);
}
void CIoEvent::UnlockOwn(void)
{
    *(this->mpuOwnLock) = 0;
}
bool CIoEvent::LockEqualOwn(void)
{
    return this->mpuLock == this->mpuOwnLock;
}

int CIoEvent::HandleTimeout( void )
{
    this->mbTimeout = false;
    return this->mpoTimerHandler->HandleTimeout();
}

int CIoEvent::PostTimeout( CTimeValue const& aoCurrentTime)
{
    this->mbTimeout = true;
    GetCycle()->GetPostDo()->PostEvent(this, MINA_POST_THREAD_EVENTS);
    return 0;
}


void CIoEvent::SetTimerHandler(CTimeoutHandler* apoHandler)
{
    this->mpoTimerHandler = apoHandler;
}


int CIoEvent::HandleInput(uint32_t auFlags)
{
    this->mpoSession->GetCycle()->GetPostDo()->PostEvent(this, auFlags);
    return 0;
}

int CIoEvent::HandleOutput(uint32_t auFlags)
{
    this->mpoSession->GetCycle()->GetPostDo()->PostEvent(this, auFlags);
    return 0;
}

int CIoTimer::HandleTimeout( void )
{
    CSession* lpoSession = NULL;
    uint32_t luIdleType;

    lpoSession = mpoIoEvent->GetSession();
    LOG_DEBUG("IDLE______________________________________");
    luIdleType = mpoIoEvent->Write() ? MINA_READ_IDLE : MINA_WRITE_IDLE;
    lpoSession->GetFilterChain()->FireSessionIdle(lpoSession, luIdleType);
    return MINA_OK;
}

MINA_NAMESPACE_END
