#include "IoService.h"
#include "Session.h"
#include "Cycle.h"

MINA_NAMESPACE_START

CIoService::CIoService(CCycle* apoCycle)
    : mpoCycle (apoCycle)
{
}


CIoService::~CIoService(void)
{
    //for()
}


void CIoService::SetHandler(CIoHandler* apoHandler)
{
    this->mpoHandler = apoHandler;
}


CIoHandler* CIoService::GetHandler(void)
{
    return this->mpoHandler;
}


void CIoService::InitSession(CSession *apoSession)
{
    apoSession->SetHandler(this->mpoHandler);
    apoSession->SetFilterChain(&this->moFilterChain);
    apoSession->SetCycle(this->mpoCycle);
    apoSession->SetService(this);

    if (this->GetConfiguration()->GetSessionPBlock())
    {
        CMemblock* lpoBlock = new CMemblock(this->GetConfiguration()->GetReadBufferSize());
        assert(lpoBlock != NULL);
        apoSession->GetRead()->SetBlock(lpoBlock);
        apoSession->GetWrite()->SetBlock(lpoBlock);
    }
    this->moSessions[apoSession->GetSessionID()] = apoSession;

}

int CIoService::InitUDP( CSockDgram& aoDgram)
{
    CIoEvent *lpoREvent, *lpoWEvent;
    CSession *lpoSession;
    CCycle* lpoCycle;
    int liSock;

    liSock = aoDgram.GetFd();
    lpoCycle = this->GetCycle();
    lpoSession = lpoCycle->GetSession(liSock);
    if (lpoSession == NULL)
    {
        if (close(liSock) == -1)
        {
            LOG_ERROR("close(%d) socket failed(%s)", liSock, strerror(errno));
        }
        return MINA_ERROR;
    }

    lpoSession->ReadActive(true);
    lpoSession->GetRead()->Accept(false);
    lpoSession->GetRead()->Channel(false);
    lpoSession->UDP(true);
    this->InitSession(lpoSession);

    if (MINA_NONBLOCKING(liSock) == -1)
    {
        LOG_ERROR("nonblocking() failed");
        lpoCycle->CloseSession(lpoSession);
        return MINA_ERROR;
    }

    lpoSession->SetSessionID(lpoCycle->GetEventDo()->GetSessionID());
    lpoSession->SetLocal(&aoDgram.GetLocal());

    lpoREvent = lpoSession->GetRead();
    lpoREvent->SetLock(&lpoSession->GetLock());
    lpoREvent->SetOwnLock(&lpoSession->GetLock());
    lpoWEvent = lpoSession->GetWrite();
    lpoWEvent->Ready(1);
    lpoWEvent->SetLock(&lpoSession->GetLock());
    lpoWEvent->SetOwnLock(&lpoSession->GetLock());

    LOG_DEBUG("created udp connection(%llu), socket %d", lpoSession->GetSessionID(), liSock);

    if (lpoCycle->GetEventDo()->GetPoll()->Add(lpoSession->GetRead(), EPOLLIN, 0) == MINA_ERROR)
    {
        lpoCycle->FreeSession(lpoSession);
        lpoSession->SetFileDesc(-1);
        close(liSock);
        return MINA_ERROR;
    }

    lpoSession->GetFilterChain()->FireSessionOpen(lpoSession);

    return MINA_OK;
}


CServiceConf* CIoService::GetConfiguration( void )
{
    return &this->moConfigure;
}

std::string& CIoService::GetName( void )
{
    return this->moName;
}

void CIoService::SetName( const char* apcName )
{
    this->moName = apcName;
}

void CIoService::SetName( std::string& aoName)
{
    this->moName = aoName;
}

CFilterChain* CIoService::GetFilterChain(void)
{
    return &this->moFilterChain;
}

CCycle* CIoService::GetCycle( void )
{
    return this->mpoCycle;
}

MINA_NAMESPACE_END
