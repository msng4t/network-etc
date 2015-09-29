#include "Poll.h"
#include "Cycle.h"


MINA_NAMESPACE_START

CPoll::CPoll(void)
    : miEpfd(-1),
      mpoEventList(NULL),
      muNevents(0)
{

}


int
CPoll::Init(size_t auMaxEvent, size_t auHint, uint32_t auMaxDelay)
{

    this->miEpfd = epoll_create(auHint / 2);
    this->muMaxDelay = auMaxDelay;
    if (this->miEpfd == -1)
    {
        LOG_ERROR("epoll_create() failed");
        return MINA_ERROR;
    }

    if (this->muNevents < auMaxEvent)
    {
        if (this->mpoEventList != NULL)
        {
            free(this->mpoEventList);
        }

        this->mpoEventList = new struct epoll_event[auMaxEvent];

        if (this->mpoEventList == NULL)
        {
            return MINA_ERROR;
        }
    }

    this->muNevents = auMaxEvent;


    return MINA_OK;
}


void
CPoll::Done(void)
{
    if (close(this->miEpfd) == -1)
    {
        LOG_ERROR("epoll close() failed");
    }

    this->miEpfd = -1;

    free(this->mpoEventList);

    this->mpoEventList = NULL;
    this->muNevents = 0;
}


int
CPoll::Add(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags)
{
    int liOption;
    uint32_t liEvents, luPrevEvents;
    CIoEvent *lpoEvent;
    CSession *lpoSession;
    struct epoll_event loEE;

    lpoSession = (CSession *)apoEvent->GetSession();
    liEvents = (uint32_t)aiEvent;

    if (aiEvent == EPOLLIN)
    {
        lpoEvent = lpoSession->GetWrite();
        luPrevEvents = EPOLLIN;
    }
    else
    {
        lpoEvent = lpoSession->GetRead();
        luPrevEvents = EPOLLOUT;
    }

    if (lpoEvent->Active())
    {
        liOption = EPOLL_CTL_MOD;
        liEvents |= luPrevEvents;
    }
    else
    {
        liOption = EPOLL_CTL_ADD;
    }

    loEE.events = liEvents | auFlags | EPOLLET;
    //loEE.data.ptr = (void*)((uintptr_t)lpoSession | apoEvent->instance);
    loEE.data.ptr = (void*)lpoSession;
    apoEvent->Active(true);
    if (epoll_ctl(this->miEpfd, liOption, lpoSession->GetFd(), &loEE) == -1)
    {
        LOG_ERROR("epoll_ctlerr(%d, %d) failed,errno: %d, info: %s", 
                liOption, lpoSession->GetFd(), errno, strerror(errno));
        if (errno == 17)
            apoEvent->Active(1);
        return MINA_ERROR;
    }
//        LOG_ERROR("epoll_ctl(%d, %d) ,errno: %d, info: %s", 
//                liOption, lpoSession->GetFd(), errno, strerror(errno));
//    LOG_DEBUG("EnableAccept ======%d===%d==========", lpoSession->GetRead(), lpoSession->GetRead()->Active());
    return MINA_OK;
}


int
CPoll::Del(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags)
{
    int liOption;
    uint32_t luPrevEvents;
    CIoEvent *lpoEvent;
    CSession *apoSession;
    struct epoll_event loEE;

    if (auFlags & 1)
    {
        apoEvent->Active(0);
        return MINA_OK;
    }

    apoSession = (CSession *)apoEvent->GetSession();

    if (aiEvent == EPOLLIN)
    {
        lpoEvent = apoSession->GetWrite();
        luPrevEvents = EPOLLOUT;
    }
    else
    {
        lpoEvent = apoSession->GetRead();
        luPrevEvents = EPOLLIN;
    }

    if (lpoEvent->Active())
    {
        liOption = EPOLL_CTL_MOD;
        loEE.events = luPrevEvents | auFlags | EPOLLET;
//        loEE.data.ptr = (void *) ((uintptr_t)apoSession | apoEvent->instance);
        loEE.data.ptr = (void *) apoSession;
    }
    else
    {
        liOption = EPOLL_CTL_DEL;
        loEE.events = 0;
        loEE.data.ptr = NULL;
    }

//    LOG_DEBUG("epoll del aiEvent:GetFd():%d liOption:%d apoEvent:%d", apoSession->GetFd(), liOption, loEE.events);

    if (epoll_ctl(this->miEpfd, liOption, apoSession->GetFd(), &loEE) == -1)
    {
        LOG_ERROR("epoll_ctl(%d,%d) failed, %s", liOption, apoSession->GetFd(), strerror(errno));
        return MINA_ERROR;
    }

 //       LOG_ERROR("epoll_ctl(%d,%d) , %s", liOption, apoSession->GetFd(), strerror(errno));
    apoEvent->Active(false);
    return MINA_OK;
}



int
CPoll::Add(CSession *apoSession)
{
    struct epoll_event loEE;
    loEE.events = EPOLLIN | EPOLLOUT | EPOLLET;
    //loEE.data.ptr = (void*)((uintptr_t)apoSession | apoSession->read->instance);
    loEE.data.ptr = (void*)apoSession;

    LOG_TRACE("epoll add connection:Epfd:%d GetFd():%d RE %p, WE %p",
            this->miEpfd, apoSession->GetFd(), apoSession->GetRead(), apoSession->GetWrite());

    if (epoll_ctl(this->miEpfd, EPOLL_CTL_ADD, apoSession->GetFd(), &loEE) == -1)
    {
        LOG_ERROR("epoll_ctl(ADD,%d) failed, %s", apoSession->GetFd(), strerror(errno));
        return MINA_ERROR;
    }
        LOG_ERROR("epoll_ctl(ADD,%d) , %s", apoSession->GetFd(), strerror(errno));

    apoSession->GetRead()->Active(true);
    apoSession->GetWrite()->Active(true);

    return MINA_OK;
}


int CPoll::Enable(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags)
{
    return this->Add(apoEvent, aiEvent, auFlags);
}
int CPoll::Disable(CIoEvent *apoEvent, int aiEvent, uint32_t auFlags)
{
    return this->Del(apoEvent, aiEvent, auFlags);
}


int CPoll::ProcessChanges(void)
{
    return 0;
}

int
CPoll::Del(CSession *apoSession, uint32_t auFlags)
{
    int liOption;
    struct epoll_event loEE;

    if (auFlags & 1)
    {
        apoSession->GetRead()->Active(0);
        apoSession->GetWrite()->Active(0);
        return MINA_OK;
    }

    liOption = EPOLL_CTL_DEL;
    loEE.events = 0;
    loEE.data.ptr = NULL;

    if (epoll_ctl(this->miEpfd, liOption, apoSession->GetFd(), &loEE) == -1)
    {
        LOG_ERROR("epoll_ctl(%d, %d) failed, %s", liOption, apoSession->GetFd(), strerror(errno));
        return MINA_ERROR;
    }

    apoSession->GetRead()->Active(0);
    apoSession->GetWrite()->Active(0);

    return MINA_OK;
}

int
CPoll::Poll(CTimer* apoTimer, uint32_t auFlags)
{
    int liEvents;
    uint32_t luEvent;
    int i;
    CIoEvent *lpoREvent, *lpoWEvent;
    CSession *apoSession;
    uint32_t luTimer = 0;
    int liError = 0;

    luTimer = apoTimer->Find();
    if (luTimer == (uint32_t)MINA_TIMER_INFINITE || luTimer > this->muMaxDelay)
    {
        luTimer = this->muMaxDelay;
    }

    liEvents = epoll_wait(this->miEpfd, this->mpoEventList, this->muNevents, luTimer);
    apoTimer->Update();
    liError = errno;

    if (((liEvents == -1) ? liError: 0) == EINTR)
    {
        LOG_ERROR("epoll_wait() failed");
        return  MINA_ERROR;
    }

    if (liEvents == 0)
    {
        if (luTimer != (uint32_t)MINA_TIMER_INFINITE)
        {
            return MINA_OK;
        }
        LOG_ERROR("epoll_wait() returned no events without timeout");
        return MINA_OK;
    }

    for (i = 0; i < liEvents; i++)
    {
        apoSession = (CSession *)this->mpoEventList[i].data.ptr;
        lpoREvent = apoSession->GetRead();
        lpoWEvent = apoSession->GetWrite();

        if (apoSession->GetFd() == -1 )
        {
            LOG_DEBUG("epoll stale aiEvent %p", apoSession);
            continue;
        }

        luEvent = this->mpoEventList[i].events;
        LOG_DEBUG("epoll wait(%s) fd(%d),Event(%u)", strerror(liError), apoSession->GetFd(), luEvent);
        if (luEvent & (EPOLLERR|EPOLLHUP))
        {
            LOG_DEBUG("epoll wait(%s) error(%d),Event(%u)", strerror(liError), apoSession->GetFd(), luEvent);
        }

        if ((luEvent & (EPOLLERR | EPOLLHUP))
                && (luEvent & (EPOLLIN | EPOLLOUT)) == 0)
        {
            LOG_DEBUG("epoll wait(%s) error(%d),Event(%u)", strerror(liError), apoSession->GetFd(), luEvent);
            luEvent |= EPOLLIN | EPOLLOUT;
        }

        if ((luEvent & EPOLLIN) && lpoREvent->Active())
        {
            lpoREvent->HandleInput(auFlags);
        }
        else if ((luEvent & EPOLLOUT) && lpoWEvent->Active())
        {
            lpoWEvent->HandleOutput(auFlags);
        }

    }

    return MINA_OK; 
}

MINA_NAMESPACE_END
