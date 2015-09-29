#include "EventDo.h"
#include "Cycle.h"
#include "WaitObject.h"

MINA_NAMESPACE_START

CEventDo::CEventDo(void)
{
}

int
CEventDo::Init( CCycle* apoCycle )
{
    CConfiguration* lpoConfig = NULL;
    size_t luMaxSession = 0;
    size_t luMaxHint = 0;
    uint32_t luMaxDelay;

    this->mpoCycle = apoCycle;
    this->mu64SessionID = 1;
    lpoConfig = apoCycle->GetConfiguration();
    luMaxDelay = lpoConfig->GetMaxDelay();
    luMaxSession = lpoConfig->GetMaxSession();
    luMaxHint = lpoConfig->GetMaxHint();

    return this->moPoll.Init(luMaxSession, luMaxHint, luMaxDelay);
}


void
CEventDo::Process(CThreadManager* apoProcessor)
{
    uint32_t luFlag = 0;
    uint32_t luDelta = 0;
    CTimer* lpoTimer = NULL;

    lpoTimer = mpoCycle->GetTimer();
    luFlag |= MINA_POST_EVENTS;

    luDelta = lpoTimer->GetMSec();
    this->moPoll.Poll(lpoTimer, luFlag);
    this->mpoCycle->ConnectAll();
    luDelta = lpoTimer->GetMSec() - luDelta;

    this->mpoCycle->GetPostDo()->ProcessAccept();

    if (luDelta)
    {
//        LOG_DEBUG("========================");
        lpoTimer->Expire();
    }

    if (this->mpoCycle->GetPostDo()->HasPosted())
    {
//        LOG_DEBUG("===WakeupThread=====================");
        apoProcessor->WakeupThread();
    }

}

void CEventDo::Accept(CIoEvent *apoEvent)
{
    socklen_t luSockLen;
    int liSock;
    int liErrno;
    CIoEvent *lpoREvent, *lpoWEvent;
    CListen *lpoListen;
    CSession *lpoSession, *lpoListenSession;
    CSockAddr loSockAddr;

    u_char lacSa[MINA_SOCKADDRLEN];

    if (apoEvent->Timeout())
    {
        LOG_DEBUG("IN Timeout Accepting");
        if (this->EnableAccept() != MINA_OK)
        {
            return;
        }

        apoEvent->Timeout(false);
    }

    lpoListenSession = apoEvent->GetSession();
    lpoListen = lpoListenSession->GetListen();
    apoEvent->Ready(0);


    do{
        luSockLen = MINA_SOCKADDRLEN;
        liSock = accept(lpoListenSession->GetFd(), (struct sockaddr *)lacSa, &luSockLen);

        if (liSock == -1)
        {
            liErrno = errno;
            if (liErrno == EAGAIN || liErrno == EINTR)
            {
                LOG_DEBUG("accept(%d) not ready", lpoListenSession->GetFd());
                break;
            }

            LOG_DEBUG("accept(%d) failed(%s)", lpoListenSession->GetFd(), strerror(liErrno));
            if (liErrno == ECONNABORTED)
            {
                break;
            }
        }

        lpoSession = mpoCycle->GetSession(liSock);
        if (lpoSession == NULL)
        {
            if (close(liSock) == -1)
            {
                LOG_ERROR("close(%d) socket failed(%s)", liSock, strerror(errno));
            }
            break;
        }

        lpoSession->GetRead()->Accept(false);
        lpoSession->GetRead()->Channel(false);
        lpoListenSession->GetService()->InitSession(lpoSession);

        if (MINA_NONBLOCKING(liSock) == -1)
        {
            LOG_ERROR("nonblocking() failed");
            mpoCycle->CloseSession(lpoSession);
            return;
        }

        lpoSession->ReadActive(true);
        lpoSession->SetListen(lpoListen);

        memcpy(lpoSession->GetRemote()->GetAddr(), lacSa, luSockLen);
        lpoSession->SetLocal(&lpoListen->GetSockAddr());

        lpoSession->SetSessionID(this->GetSessionID());

        lpoREvent = lpoSession->GetRead();
        lpoREvent->SetLock(&lpoSession->GetLock());
        lpoREvent->SetOwnLock(&lpoSession->GetLock());
        lpoWEvent = lpoSession->GetWrite();
        lpoWEvent->Ready(1);
        lpoWEvent->SetLock(&lpoSession->GetLock());
        lpoWEvent->SetOwnLock(&lpoSession->GetLock());

        LOG_DEBUG("accept remote(%s:%d) connection(%d), socket %d"
                , lpoSession->GetRemote()->GetIP(), lpoSession->GetRemote()->GetPort()
                , lpoSession->GetSessionID(), liSock);

        if (this->moPoll.Add(lpoSession->GetRead(), EPOLLIN, 0) == MINA_ERROR)
        {
            this->CloseAccepted(lpoSession);
            return;
        }

        lpoSession->GetFilterChain()->FireSessionOpen(lpoSession);

    }while(true);

}


int CEventDo::DisableAccept(void)
{
    CListen *lpoListen;
    CSession *lpoSession;
    CIoAcceptor *lpoService;
    int liCounter = 0;

    do
    {
        lpoService = this->mpoCycle->GetAcceptor(liCounter++);
        if (lpoService == NULL)
        {
            break;
        }
        lpoListen = lpoService->GetListen();
        lpoSession = lpoListen->GetSession();
        if (lpoSession->GetRead()->Active())
        {
            if (this->moPoll.Del(lpoSession->GetRead(), EPOLLIN, 0) == MINA_ERROR)
            {
                return MINA_ERROR;
            }
        }
    }while(true);
    return MINA_OK;

}

int CEventDo::EnableAccept(void)
{
    CListen* lpoListen;
    CSession* lpoSession;
    CIoAcceptor *lpoService;
    int liCounter = 0;

    do
    {
        lpoService = this->mpoCycle->GetAcceptor(liCounter++);
        if (lpoService == NULL)
        {
            break;
        }
        lpoListen = lpoService->GetListen();
        lpoSession = lpoListen->GetSession();
        if (!lpoSession->GetRead()->Active())
        {
            if (this->moPoll.Add(lpoSession->GetRead(), EPOLLIN, 0) == MINA_ERROR)
            {
                return MINA_ERROR;
            }
        }
    }while(true);

    return MINA_OK;
}


void CEventDo::CloseAccepted(CSession *apoSession)
{
    int liFd;

    mpoCycle->FreeSession(apoSession);
    liFd = apoSession->GetFd();
    apoSession->SetFileDesc(-1);

    if (close(liFd) == -1)
    {
        LOG_ERROR("close socket failed(%s)", strerror(errno));
    }
}

uint64_t CEventDo::GetSessionID(void)
{
    return MinaAtomicFetchAdd(&this->mu64SessionID, 1);
}

int
CEventDo::Read(CIoEvent *apoEvent)
{
    CSession* lpoSession;
    CMemblock* lpoBlock;
    int liReadSize;

    lpoSession = apoEvent->GetSession();
    lpoBlock = apoEvent->GetBlock();
    if (lpoBlock->GetSpace() == 0)
    {
        lpoBlock->Resize(lpoBlock->GetCapacity() * 2);
    }

    do
    {
        if (lpoSession->UDP())
        {
            liReadSize = COsAction::RecvUDP(lpoSession, lpoBlock->GetWrite(), lpoBlock->GetDirectSpace());
        }
        else
        {
            liReadSize = COsAction::Recv(lpoSession, lpoBlock->GetWrite(), lpoBlock->GetDirectSpace());
        }
        if (liReadSize <= 0)
        {
            if (liReadSize == MINA_AGAIN || liReadSize == EINTR)
            {
                return liReadSize;
            }

            LOG_DEBUG("CloseSession.....");
            lpoSession->GetFilterChain()->FireFilterClose(lpoSession);
            mpoCycle->CloseSession(lpoSession);

            return MINA_ERROR;
        }

        lpoBlock->SetWrite(liReadSize);
        if (lpoBlock->GetDirectSpace() == 0)
        {
            if (lpoBlock->GetSpace() > (size_t)liReadSize)
            {
                lpoBlock->Shrink();
            }
            else
            {
                lpoBlock->Resize(lpoBlock->GetCapacity() * 2);
            }
            continue;
        }

        CAny loMessage(lpoBlock);
        lpoSession->GetFilterChain()->FireMessageRecived(lpoSession, loMessage);
    }while(false);

    return MINA_OK;
}

int
CEventDo::Check(CIoEvent *apoEvent)
{
    CSession* lpoSession;
    int liRetCode;

    lpoSession = apoEvent->GetSession();
    mpoCycle->GetSessionManager()->CheckOne(lpoSession->GetService()->GetName());
    liRetCode = COsAction::CheckStatus(lpoSession);
    if (liRetCode != 0)
    {
        LOG_ERROR("Connecting failed(%s)", strerror(liRetCode));
        lpoSession->GetCycle()->CloseSession(lpoSession);
        return MINA_ERROR;
    }

    lpoSession->ReadActive(true);
    lpoSession->GetFilterChain()->FireSessionOpen(lpoSession);
    return MINA_OK;
}

int
CEventDo::Write(CIoEvent *apoEvent, bool abPost)
{
    CSession* lpoSession;
    CMemblock* lpoBlock;
    int liWriteSize;

    lpoSession = apoEvent->GetSession();
    lpoBlock = apoEvent->GetBlock();
    if (lpoBlock->GetSize() == 0)
    {
        if (abPost)
        {
            this->moPoll.Del(apoEvent, EPOLLOUT, 0);
        }
        return MINA_OK;
    }

    if (lpoSession->UDP())
    {
        liWriteSize = COsAction::SendUDP(lpoSession, lpoBlock->GetRead(), lpoBlock->GetSize());
        LOG_TRACE("Send %s:%d", lpoBlock->GetRead());
    }
    else
    {
        if (lpoSession->GetService()->GetConfiguration()->IsDirectWrite())
        {
            liWriteSize = COsAction::Send(lpoSession, lpoBlock->GetRead(), lpoBlock->GetSize());
        }
    }
    lpoBlock->SetRead(liWriteSize);

    if (lpoBlock->GetSize() != 0)
    {
        if (errno == EAGAIN || errno == EINTR)
        {
            return MINA_AGAIN;
        }

        lpoSession->GetFilterChain()->FireFilterClose(lpoSession);
        mpoCycle->CloseSession(lpoSession);

        return MINA_ERROR;
    }
    else
    {
        lpoBlock->Reset();
        if (abPost)
        {
            this->moPoll.Del(apoEvent, EPOLLOUT, 0);
        }
    }

    return MINA_OK;
}


CPoll* CEventDo::GetPoll(void)
{
    return &this->moPoll;
}


MINA_NAMESPACE_END
