#include "OsAction.h"
#include "Log.h"
#include "Singleton.h"

MINA_NAMESPACE_START

ssize_t
COsAction::Recv(CSession *apoSession, char *apcBuffer, size_t auSize)
{
    ssize_t liSize;
    int liError;
    CIoEvent *lpoREvent;

    lpoREvent = apoSession->GetRead();

    do{
        liSize = recv(apoSession->GetFd(), apcBuffer, auSize, 0);
        if (liSize == 0)
        {
            lpoREvent->Ready(0);
            return liSize;
        }
        else if (liSize > 0)
        {
            if ((size_t)liSize <auSize)
            {
                lpoREvent->Ready(0);
            }

            return liSize;
        }

        liError = errno;
        if (liError == EAGAIN || liError == EINTR)
        {
            LOG_DEBUG("recv() not ready");
            liSize = MINA_AGAIN;
        }
        else
        {
            liSize = MINA_ERROR;
            break;
        }
    }while (liError == EINTR);

    lpoREvent->Ready(0);

    return liSize;
}

ssize_t
COsAction::RecvUDP(CSession *apoSession, char *apcBuffer, size_t auSize)
{
    ssize_t liSize;
    int liError;
    CIoEvent *lpoREvent;
    socklen_t luSockLen = apoSession->GetLocal()->GetLength();
    struct sockaddr* lpoAddr = apoSession->GetLocal()->GetAddr();
    lpoREvent = apoSession->GetRead();

    do{
        liSize = recvfrom(apoSession->GetFd(), apcBuffer, auSize, 0
                , lpoAddr, &luSockLen);

        liError = errno;
        LOG_ERROR("Recv %d, errno %d", liSize, errno);
        if (liSize > 0)
        {
            return liSize;
        }


        if (liError == EAGAIN || liError == EINTR)
        {
            LOG_DEBUG("recv() not ready");
            liSize = MINA_AGAIN;
        }
        else
        {
            liSize = MINA_ERROR;
            break;
        }
    }while (liError == EINTR);

    lpoREvent->Ready(0);

    return liSize;
}


ssize_t
COsAction::RecvChain(CSession *apoSession, char *apcBuffer)
{
    LOG_ERROR("readv_chain() not formed yet");
    return 0;
}


ssize_t
COsAction::Send(CSession *apoSession, char *apcBuffer, size_t auSize)
{
    ssize_t liSize;
    int liError;
    CIoEvent *lpoWEvent;

    lpoWEvent = apoSession->GetWrite();

    for (;;)
    {
        liSize = send(apoSession->GetFd(), apcBuffer, auSize, 0);
        LOG_DEBUG("Send size: %d:%s of %d", liSize, apcBuffer, auSize);
        if (liSize >0)
        {
            if ((size_t)liSize < auSize)
            {
                lpoWEvent->Ready(0);
            }
            return liSize;
        }

        liError = errno;
        if (liSize == 0)
        {
            LOG_ERROR("send() returned zero");
            lpoWEvent->Ready(0);
            return liSize;
        }

        if (liError == EAGAIN || liError == EINTR)
        {
            lpoWEvent->Ready(0);
            LOG_DEBUG("send() not ready");

            if (liError == EAGAIN)
            {
                return MINA_AGAIN;
            }
        }
        else
        {
            liSize = MINA_ERROR;
            return MINA_ERROR;
        }
    }
}

ssize_t
COsAction::SendUDP(CSession *apoSession, char *apcBuffer, size_t auSize)
{
    ssize_t liSize;
    int liError;
    CIoEvent *lpoWEvent;

    socklen_t luSockLen = apoSession->GetDest()->GetLength();
    struct sockaddr* lpoAddr = apoSession->GetDest()->GetAddr();
    lpoWEvent = apoSession->GetWrite();

    for (;;)
    {
        liSize = sendto(apoSession->GetFd(), apcBuffer, auSize, 0
                , lpoAddr, luSockLen);
        LOG_DEBUG("Send size: %d:%s of %d", liSize, apcBuffer, auSize);
        if (liSize >0)
        {
            if ((size_t)liSize < auSize)
            {
                lpoWEvent->Ready(0);
            }
            return liSize;
        }

        liError = errno;
        if (liSize == 0)
        {
            LOG_ERROR("send() returned zero");
            lpoWEvent->Ready(0);
            return liSize;
        }

        if (liError == EAGAIN || liError == EINTR)
        {
            lpoWEvent->Ready(0);
            LOG_DEBUG("send() not ready");

            if (liError == EAGAIN)
            {
                return MINA_AGAIN;
            }
        }
        else
        {
            liSize = MINA_ERROR;
            return MINA_ERROR;
        }
    }
}

ssize_t
COsAction::SendChain(CSession *apoSession, char *apcBuffer)
{
    LOG_ERROR("writev_chain() not formed yet");
    return 0;
}


int COsAction::CheckStatus(CSession *apoSession)
{
    /*
       int liError = -1;
       socklen_t luSockLen;

       if (getsockopt(apoSession->GetFd(), SOL_SOCKET, SO_ERROR, (void*)&liError, &luSockLen) == -1)
       {
       liError = errno;
       }
       return liError;
       */

    struct sockaddr loSockAddr;
    socklen_t liSockLen = sizeof(loSockAddr);
    if (getpeername(apoSession->GetFd(), &loSockAddr, &liSockLen) != 0)
    {
        return MINA_ERROR;
    }

    return 0;
}

MINA_NAMESPACE_END
