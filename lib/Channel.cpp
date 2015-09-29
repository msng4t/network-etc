#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "Channel.h"
#include "Log.h"
#include "Singleton.h"
#include "IoEvent.h"
#include "Session.h"
MINA_NAMESPACE_START


int
MinaWriteChannel(int aiSock, SChannel *lpoChanel)
{
    ssize_t luSize;
    int liErrno;
    struct iovec loIovec[1];
    struct msghdr loMsg;
    union{
        struct cmsghdr cm;
        char space[CMSG_SPACE(sizeof(int))];
    }cmsg;

    if (lpoChanel->miFd == -1)
    {
        loMsg.msg_control = NULL;
        loMsg.msg_controllen = 0;
    }
    else
    {
        //memset(&cmsg, 0, sizeof(cmsg));
        loMsg.msg_control = (caddr_t) &cmsg;
        loMsg.msg_controllen = sizeof(cmsg);

        cmsg.cm.cmsg_len = CMSG_LEN(sizeof(int));
        cmsg.cm.cmsg_level = SOL_SOCKET;
        cmsg.cm.cmsg_type = SCM_RIGHTS;

        memcpy(CMSG_DATA(&cmsg.cm), &lpoChanel->miFd, sizeof(int));
    }

    loMsg.msg_flags = 0;

    loIovec[0].iov_base = (char *)lpoChanel;
    loIovec[0].iov_len = sizeof(SChannel);

    loMsg.msg_name = NULL;
    loMsg.msg_namelen = 0;
    loMsg.msg_iov = loIovec;
    loMsg.msg_iovlen = 1;

    luSize = sendmsg(aiSock, &loMsg, 0);

    if (luSize == -1)
    {
        liErrno = errno;
        if (liErrno == EAGAIN)
        {
            return MINA_AGAIN;
        }

        LOG_ERROR("%d sendmsg() fail use channel %d errno: %d, info: %s", getpid(), aiSock,errno, strerror(errno));
        return MINA_ERROR;
    }
    LOG_ERROR("%d sendmsg() use channel %d finished", getpid(), aiSock);
    return MINA_OK;
}


int
MinaReadChannel(int aiSock, SChannel *lpoChanel)
{
    ssize_t luSize;
    int liErrno;
    struct iovec loIovec[1];
    struct msghdr loMsg;
    union{
        struct cmsghdr cm;
        char space[CMSG_SPACE(sizeof(int))];
    }cmsg;

    //memset(&loMsg, 0, sizeof(loMsg));
    loIovec[0].iov_base = (char *) lpoChanel;
    loIovec[0].iov_len = sizeof(SChannel);
    loMsg.msg_name = NULL;
    loMsg.msg_namelen = 0;
    loMsg.msg_iov = loIovec;
    loMsg.msg_iovlen = 1;

    loMsg.msg_control = (caddr_t)&cmsg;
    loMsg.msg_controllen = sizeof(cmsg);

    luSize = recvmsg(aiSock, &loMsg, 0);
        liErrno = errno;

    LOG_ERROR("%d %u readmsg() use channel %d ", getpid(), pthread_self(), aiSock);
    if (luSize == -1)
    {
        if (liErrno == EAGAIN)
        {
            return MINA_AGAIN;
        }

        LOG_ERROR("recvmsg() failed errno: %d, info: %s", liErrno, strerror(liErrno));
        return MINA_ERROR;
    }

    if (luSize == 0)
    {
        LOG_DEBUG("recvmsg return zero");
        return MINA_ERROR;
    }
        LOG_DEBUG("recvmsg return %d, size: %d", luSize, sizeof(SChannel));

    if ((size_t)luSize < sizeof(SChannel))
    {
        LOG_ERROR("recvmsg() returned not enough data: %d",luSize);;
        return MINA_ERROR;
    }

    if(lpoChanel->muCommand == MINA_CMD_OPEN_CHANNEL)
    {
        if (cmsg.cm.cmsg_len < (socklen_t)CMSG_LEN(sizeof(int)))
        {
            LOG_ERROR("recvmsg() returned too small ancillary data");
            return MINA_ERROR;
        }

        if (cmsg.cm.cmsg_level != SOL_SOCKET || cmsg.cm.cmsg_type != SCM_RIGHTS)
        {
            LOG_ERROR("recvmsg() returned invalid ancillary data"
                    "level %d or type %d",
                    cmsg.cm.cmsg_level,
                    cmsg.cm.cmsg_type);
            return MINA_ERROR;
        }

        memcpy(&lpoChanel->miFd, CMSG_DATA(&cmsg.cm), sizeof(int));
    }

    if (loMsg.msg_flags & (MSG_TRUNC | MSG_CTRUNC))
    {
        LOG_ERROR("recvmsg() truncated data");
    }
    LOG_ERROR("recvmsg() finished");
    return luSize;
}


int
MinaAddChannelEvent(CCycle* apoCycle, int aiSock, int aiEvent)
{
    CIoEvent *lpoEvent, *lpoREvent, *lpoWEvent;
    CSession *lpoSession;
    
    LOG_DEBUG("channel");
    lpoSession = apoCycle->GetSession(aiSock);

    if (lpoSession == NULL)
    {
        LOG_ERROR("add_channel_event() fail");
        return MINA_ERROR;
    }

    lpoREvent = lpoSession->GetRead();
    lpoWEvent = lpoSession->GetWrite();
    lpoREvent->SetLock(&lpoSession->GetLock());
    lpoWEvent->SetLock(&lpoSession->GetLock());
    lpoREvent->SetOwnLock(&lpoSession->GetLock());
    lpoWEvent->SetOwnLock(&lpoSession->GetLock());
    lpoREvent->Channel(true);
    lpoWEvent->Channel(true);

    lpoEvent = (aiEvent == EPOLLIN)? lpoREvent : lpoWEvent;

    LOG_DEBUG("lpoEvent:%d, channel:%d", lpoEvent, lpoEvent->Channel());
    if (apoCycle->GetEventDo()->GetPoll()->Add(lpoSession->GetRead(), EPOLLIN, 0) == MINA_ERROR)
    {
        LOG_DEBUG("add channel aiEvent fail");
        apoCycle->FreeSession(lpoSession);
        return MINA_ERROR;
    }
    LOG_DEBUG("add channel aiEvent success");

    return MINA_OK; 
}


void
MinaCloseChannel(int *aiFd)
{
    if (close(aiFd[0]) == -1)
    {
        LOG_ERROR("close() channel failed");
    }

    if (close(aiFd[1]) == -1)
    {
        LOG_ERROR("close() channel failed");
    }
}


void MinaChannelHandler(CIoEvent* apoEvent)
{
    SChannel loChannel;
    MinaReadChannel(apoEvent->GetSession()->GetFd(), &loChannel);

}

MINA_NAMESPACE_END
