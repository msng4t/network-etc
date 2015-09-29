#include "Cycle.h"
#include "Log.h"
#include "Singleton.h"
#include "Timer.h"
#include "EventDo.h"
#include "IoAcceptor.h"
#include "IoConnector.h"
#include "Channel.h"

MINA_NAMESPACE_START
CCycle* gpoCycle = NULL;

CCycle* GetCycle(void)
{
    if (gpoCycle == NULL)
        gpoCycle = new CCycle;
    return gpoCycle;
}

CCycle::CCycle(void)
    : mpoFreeSessions (NULL)
    , muFreeCount (0)
    , muMaxFd (0)
    , mpoSessions (NULL)
    , mpoREventPtrs (NULL)
    , mpoWEventPtrs (NULL)
    , miCurAcceptors (0)
    , miCurConnectors (0)
    , miCurMaxAcceptors (ACCEPTORSIZE)
    , miCurMaxConnectors (ACCEPTORSIZE)
    , mpoMessageTable (NULL)
    , mpoSessionManager (NULL)
{
    memset(this->mapoConnectors, 0, sizeof(this->mapoConnectors));
    memset(this->mapoAcceptors, 0, sizeof(this->mapoAcceptors));
    this->mppoAcceptors = this->mapoAcceptors;
    this->mppoConnectors = this->mapoConnectors;

}

CCycle::~CCycle()
{
    delete[] this->mpoSessions;
    delete[] this->mpoREventPtrs;
    delete[] this->mpoWEventPtrs;
}

int
CCycle::Init(void)
{
    int liLimit;
    struct rlimit rlimit;

    if (getrlimit(RLIMIT_NOFILE, &rlimit) == -1)
    {
        LOG_ERROR("getrlimit(RLIMIT_NOFILE) failed, ignored");
    }
    else
    {
        liLimit = rlimit.rlim_cur;
        if (this->moConfiguration.GetMaxSession() > (uint32_t) rlimit.rlim_cur)
        {
            LOG_ERROR("%d worker_connections exceed open file resource liLimit:%d ",
                    this->moConfiguration.GetMaxSession(), liLimit);
            this->moConfiguration.SetMaxSession(rlimit.rlim_cur);
        }
        this->muMaxFd= (uint32_t)rlimit.rlim_cur;
    }

    if (this->moTimer.Init() != MINA_OK)
    {
        return MINA_ERROR;
    }
    this->moPostDo.SetCycle(this);

    this->moTimer.Update();
    if (this->InitEvent())
    {
        return MINA_ERROR;
    }

    if (this->moEventDo.Init(this) != MINA_OK)
    {
        return MINA_ERROR;
    }

    if (this->moPair.Open())
    {
        return MINA_ERROR;
    }

    if (MinaAddChannelEvent(this, this->moPair.GetSecond(), EPOLLIN) != MINA_OK)
    {
        return MINA_ERROR;
    }

    return MINA_OK;
}

int
CCycle::InitEvent(void)
{
    uint32_t luCount;
    CIoEvent *lpoREvent, *lpoWEvent;
    CSession *lpoSession, *lpoNext;

    luCount = this->moConfiguration.GetMaxSession();
    lpoSession = new CSession[luCount];
    this->mpoSessions = lpoSession;

    lpoREvent = new CIoEvent[luCount];
    this->mpoREventPtrs = lpoREvent;
    lpoWEvent = new CIoEvent[luCount];
    this->mpoWEventPtrs = lpoWEvent;

    if (this->mpoSessions == NULL)
    {
        return MINA_ERROR;
    }
    if (this->mpoREventPtrs == NULL)
    {
        return MINA_ERROR;
    }
    if (this->mpoWEventPtrs == NULL)
    {
        return MINA_ERROR;
    }

    this->muFreeCount = luCount;
    lpoNext = NULL;

    do{
        luCount--;
        lpoWEvent[luCount].Closed(1);
        lpoWEvent[luCount].SetLock (&lpoSession[luCount].GetLock());
        lpoWEvent[luCount].SetOwnLock (&lpoSession[luCount].GetLock());
        lpoWEvent[luCount].SetTimerHandler(new CIoTimer(&lpoWEvent[luCount]));

        lpoREvent[luCount].Closed(1);
        lpoREvent[luCount].SetLock (&lpoSession[luCount].GetLock());
        lpoREvent[luCount].SetOwnLock (&lpoSession[luCount].GetLock());
        lpoREvent[luCount].SetTimerHandler(new CIoTimer(&lpoREvent[luCount]));

        lpoSession[luCount].SetData( lpoNext);
        lpoSession[luCount].SetRead (&this->mpoREventPtrs[luCount]);
        lpoSession[luCount].SetWrite(&this->mpoWEventPtrs[luCount]);
        lpoSession[luCount].SetFileDesc(-1);

        lpoNext = &lpoSession[luCount];
        lpoSession[luCount].SetLock(0);
    }while(luCount);

    this->mpoFreeSessions = lpoNext;


    return MINA_OK;

}

CConfiguration* CCycle::GetConfiguration(void)
{
    return &this->moConfiguration;
}

uint32_t
CCycle::GetSessionN(void)
{
    return this->moConfiguration.GetMaxSession();
}

uint32_t CCycle::GetFreeSessionN(void)
{
    return this->muFreeCount;
}

CSession*
CCycle::GetSessions(void)
{
    return this->mpoSessions;
}

CIoEvent*
CCycle::GetReads(void)
{
    return this->mpoREventPtrs;
}

CIoEvent*
CCycle::GetWrites(void)
{
    return this->mpoWEventPtrs;
}

CSession*
CCycle::GetFreeSessions(void)
{
    return this->mpoFreeSessions;
}

CSession*
CCycle::GetSession(int aiSock)
{
    CIoEvent *loREvent;
    CIoEvent *loWEvent;
    CSession *lpoSession;

    LOG_DEBUG("aiSock: %d muMaxFd: %d", aiSock, this->muMaxFd);
    if ((uint32_t)aiSock >= this->muMaxFd)
    {
        return NULL;
    }

    lpoSession = this->mpoFreeSessions;

    if (lpoSession == NULL)
    {
        LOG_ERROR("Worker_session are not enough");
        return NULL;
    }

    this->mpoFreeSessions = (CSession *)lpoSession->GetData();
    this->muFreeCount--;

    loREvent = lpoSession->GetRead();
    loWEvent = lpoSession->GetWrite();

    LOG_DEBUG("connections: %d connection: %d", this->muFreeCount, lpoSession);

    lpoSession->Reset();

    lpoSession->SetRead(loREvent);
    lpoSession->SetWrite(loWEvent);
    lpoSession->SetFileDesc(aiSock);
    lpoSession->SetSessionID(this->moEventDo.GetSessionID());
    lpoSession->SetCycle(this);


    loREvent->Reset();
    loWEvent->Reset();



    loREvent->SetSession(lpoSession);
    loWEvent->SetSession(lpoSession);
    loREvent->Write(false);
    loWEvent->Write(true);

    loREvent->SetLock(&lpoSession->GetLock());
    loREvent->SetOwnLock(&lpoSession->GetLock());
    loWEvent->SetLock(&lpoSession->GetLock());
    loWEvent->SetOwnLock(&lpoSession->GetLock());
    return lpoSession;
}


void
CCycle::FreeSession(CSession *apoSession)
{
    apoSession->SetData(this->mpoFreeSessions);
    this->mpoFreeSessions = apoSession;
    this->muFreeCount++;

}

void
CCycle::CloseSession(CSession *apoSession)
{
    int liErrno;
    int liFd;

    apoSession->Close();
    if (apoSession->GetFd() == -1)
    {
        LOG_ERROR("connection already closed");
        return;
    }
    this->moEventDo.GetPoll()->Del(apoSession, 0);

/*  to EventPosted.cpp */
//##     this->moLock.Lock();
//## 
//##     if ( apoSession->GetRead()->prev)
//##     {
//##         this->LockedDeletePosted(apoSession->GetRead());
//##     }
//## 
//##     if ( apoSession->GetWrite()->prev)
//##     {
//##         this->LockedDeletePosted(apoSession->GetWrite());
//##     }
//##     this->moLock.Unlock();

    apoSession->GetRead()->Closed(1);
    apoSession->GetWrite()->Closed(1);


    apoSession->GetRead()->Locked(0);
    apoSession->GetWrite()->Locked(0);
    if (apoSession->GetRemote() != NULL)
    {
//        delete apoSession->GetRemote();
//##        apoSession->SetRemote(NULL);
    }

    if (apoSession->GetLocal() != NULL)
    {
//        delete apoSession->GetLocal();
        apoSession->SetLocal(NULL);
    }

    this->FreeSession(apoSession);

    liFd = apoSession->GetFd();
    apoSession->SetFileDesc(-1);

    if (close(liFd) == -1)
    {
        liErrno = errno;

        if (liErrno == ECONNRESET || liErrno == ENOTCONN)
        {
        }

        LOG_ERROR("close socket %d failed: %s", liFd, strerror(liErrno));
    }

}

CTimer* CCycle::GetTimer(void)
{
    return &this->moTimer;
}


CEventDo* CCycle::GetEventDo(void)
{
    return &this->moEventDo;
}

CEventPosted* CCycle::GetPostDo(void)
{
    return &this->moPostDo;
}

MessageTableType* CCycle::GetMessageTable(void)
{
    if (this->mpoMessageTable == NULL)
    {
        this->mpoMessageTable = new MessageTableType;
        assert(this->mpoMessageTable != NULL);
    }
    return this->mpoMessageTable;
}

void CCycle::SetMessageTable(MessageTableType* apoTable)
{
    this->mpoMessageTable = apoTable;
}

CSessionManager* CCycle::GetSessionManager(void)
{
    if (this->mpoSessionManager == NULL)
    {
        this->mpoSessionManager = new CSessionManager;
        assert(this->mpoSessionManager != NULL);
    }
    return this->mpoSessionManager;
}

void CCycle::SetSessionManager(CSessionManager* apoManager)
{
    this->mpoSessionManager = apoManager;
}



void CCycle::SetAcceptor(CIoAcceptor *apoAcceptor)
{
    CIoAcceptor** lppoPose;

    GETEXARRYENTRY(CIoAcceptor*, this->miCurAcceptors, this->miCurMaxAcceptors,
            this->mppoAcceptors, this->mapoAcceptors, lppoPose);
    *lppoPose = apoAcceptor;
    apoAcceptor->GetConfiguration()->Set(*this->GetConfiguration());
}

CIoAcceptor* CCycle::GetAcceptor( int aiNum )
{
    return this->mppoAcceptors[aiNum];
}

void CCycle::SetConnector(CIoConnector *apoConnector)
{
    CIoConnector** lppoPose;

    GETEXARRYENTRY(CIoConnector*, this->miCurConnectors, this->miCurMaxConnectors,
            this->mppoConnectors, this->mapoConnectors, lppoPose);
    *lppoPose = apoConnector;

}

void CCycle::ConnectAll(void)
{
    for (int liN = 0; liN < this->miCurConnectors; liN++)
    {
        if (this->mppoConnectors[liN] != NULL)
        {
            if (!this->mppoConnectors[liN]->GetConnected())
            {
                this->mppoConnectors[liN]->Connect();
            }
            else
            {
                this->mppoConnectors[liN]->HandleChanged();
            }
        }
    }
}

void CCycle::Wakeup(void)
{
    SChannel loCh;
    MinaWriteChannel(this->moPair.GetFirst(), &loCh);

}

MINA_NAMESPACE_END
