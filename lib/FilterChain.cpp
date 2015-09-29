#include "FilterChain.h"
#include "Queue.h"
#include "Log.h"
#include "Singleton.h"
#include "OsAction.h"
#include "SessionManager.h"
#include "Cycle.h"


MINA_NAMESPACE_START

CFilterChain::CFilterChain(void)
{
    this->Init();
}

int
CFilterChain::Init(void)
{
    CFilter *apoFilter;
    CNextFilter *lpoNextFilter;

    moHead.Init();
    apoFilter = new CFilterHead();

    if (apoFilter == NULL)
    {
        return MINA_ERROR;
    }
    lpoNextFilter = new CNextFilter("Head", apoFilter);
    moHead.InsertAM(lpoNextFilter);

    apoFilter = new CFilterTail();

    if (apoFilter == NULL)
    {
        return MINA_ERROR;
    }
    lpoNextFilter = new CNextFilter("Tail", apoFilter);
    moHead.InsertBM(lpoNextFilter);


    return MINA_OK;
}


void
CFilterChain::AddFirst(const char *apcName, CFilter *apoFilter)
{
    CNextFilter *lpoNextFilter;
    lpoNextFilter = new CNextFilter(apcName, apoFilter);
    moHead.Next()->InsertAM(lpoNextFilter);
//    MINA_QUEUE_INSERT_AFTER(MINA_QUEUE_HEAD(&moHead), lpoNextFilter);

}
void
CFilterChain::AddLast(const char *apcName, CFilter *apoFilter)
{
    CNextFilter *lpoNextFilter;
    lpoNextFilter = new CNextFilter(apcName, apoFilter);
    moHead.Prev()->InsertBM(lpoNextFilter);
//    MINA_QUEUE_INSERT_AFTER(MINA_QUEUE_HEAD(&moHead)->prev, lpoNextFilter);

}

void
CFilterChain::FireSessionOpen(SessionType aoSession)
{
    CCycle* lpoCycle = NULL;
    CSessionManager* lpoManager = NULL;

    lpoCycle = aoSession->GetCycle();
    lpoManager = lpoCycle->GetSessionManager();
    lpoManager->Insert(aoSession);
    lpoCycle->GetTimer()->Add(aoSession->GetWrite(),
            TIME_MSEC * aoSession->GetService()->GetConfiguration()->GetWriteIdle());
    lpoCycle->GetTimer()->Add(aoSession->GetRead(),
            TIME_MSEC * aoSession->GetService()->GetConfiguration()->GetReadIdle());

    //CQueue* lpoHead = MINA_QUEUE_HEAD(&moHead);
    //CNextFilter* lpoNextFilter = MINA_QUEUE_DATA(lpoHead, CNextFilter, moQueue);
    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;

    //lpoHead = MINA_QUEUE_NEXT(lpoHead);
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
//    lpoNextFilter = MINA_QUEUE_DATA(lpoHead, CNextFilter, moQueue);
    lpoFilter->SessionOpen( aoSession, lpoNextFilter);
}

void CFilterChain::FireSessionClose(SessionType aoSession)
{
    CSessionManager* lpoManager = NULL;

    lpoManager = aoSession->GetCycle()->GetSessionManager();
    lpoManager->Erase(aoSession);

    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
    lpoFilter->SessionClose( aoSession, lpoNextFilter);

}
void CFilterChain::FireSessionIdle(SessionType aoSession, uint32_t auStatus)
{
    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
    lpoFilter->SessionIdle(aoSession, auStatus, lpoNextFilter);
}
void CFilterChain::FireMessageRecived(SessionType aoSession, CAny& aoMessage)
{
    CTimer* lpoTimer = aoSession->GetCycle()->GetTimer();
    lpoTimer->Add(aoSession->GetRead(),
            TIME_MSEC * aoSession->GetService()->GetConfiguration()->GetWriteIdle());

    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
    lpoFilter->MessageRecived(aoSession, aoMessage, lpoNextFilter);
}


void CFilterChain::FireMessageFinished(SessionType aoSession, CAny& aoMessage)
{
    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
    lpoFilter->MessageFinished(aoSession, aoMessage, lpoNextFilter);

}

void CFilterChain::FireFilterWrite(SessionType aoSession, CAny& aoMessage)
{
    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
    lpoFilter->FilterWrite(aoSession, aoMessage, lpoNextFilter);
}

void CFilterChain::FireFilterClose(SessionType aoSession)
{
    CTimer* lpoTimer = aoSession->GetCycle()->GetTimer();
    if (aoSession->GetRead()->TimerOn() )
    {
        lpoTimer->Delete(aoSession->GetRead());
    }
    if (aoSession->GetWrite()->TimerOn() )
    {
        lpoTimer->Delete(aoSession->GetWrite());
    }

    CNextFilter* lpoNextFilter = moHead.Next()->Data<CNextFilter>();
    CFilter* lpoFilter = lpoNextFilter->mpoFilter;
    lpoNextFilter = moHead.Next()->Next()->Data<CNextFilter>();
    lpoFilter->FilterClose(aoSession, lpoNextFilter);
}



void CFilterChain::Response(SessionType aoSession, CAny& aoMessage)
{
    this->FireFilterWrite(aoSession, aoMessage);

    this->FireMessageFinished(aoSession, aoMessage);
}

CFilterChain::~CFilterChain(void)
{
    CQueueNode *lpoFilter = NULL;
    CQueueNode *lpoNextFilter = NULL;
    CNextFilter *lpoFilterData = NULL;

    lpoFilter = moHead.Next();
    while(lpoFilter != &moHead)
    {
        lpoNextFilter = lpoFilter->Next();
        lpoNextFilter->Remove(lpoFilter);
        lpoFilterData = lpoFilter->Data<CNextFilter>();
        lpoFilter = lpoNextFilter;
        delete lpoFilterData->mpoFilter;
        delete lpoFilterData;
    }
}

void
CFilterTail::SessionOpen(SessionType aoSession, CNextFilter *apoFilter)
{
    //LOG_DEBUG("test SessionOpen_tail");
    CIoHandler* lpoHandler = aoSession->GetHandler();
    lpoHandler->SessionOpen(aoSession);
}
void
CFilterTail::SessionClose(SessionType aoSession, CNextFilter *apoFilter)
{
    //LOG_DEBUG("test SessionClose_tail");
    
    CIoHandler* lpoHandler = aoSession->GetHandler();
    lpoHandler->SessionClose(aoSession);
}
void
CFilterTail::SessionIdle(SessionType aoSession, uint32_t auStatus, CNextFilter *apoFilter)
{
    //LOG_DEBUG("test filter_idlee_tail");

    CIoHandler* lpoHandler = aoSession->GetHandler();
    lpoHandler->SessionIdle(aoSession, auStatus);

}
void
CFilterTail::MessageRecived(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    //LOG_DEBUG("test filter_recived_tail");

    CIoHandler* lpoHandler = aoSession->GetHandler();
    assert(lpoHandler != NULL);
    lpoHandler->MessageRecived(aoSession, aoMessage);

}
void
CFilterTail::MessageFinished(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{

    CIoHandler* lpoHandler = aoSession->GetHandler();
    lpoHandler->MessageFinished(aoSession, aoMessage);
}
void
CFilterTail::FilterClose(SessionType aoSession, CNextFilter *apoFilter)
{
    //LOG_DEBUG("test FilterClose_tail");

    apoFilter->FilterClose(aoSession);

}
void
CFilterTail::FilterWrite(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    size_t luSize;
    CMemblock *lpoBlock;
    CMemblock *lpoMessage;
    CEventDo *lpoEventDo;
    int liRet = -1;

    lpoMessage = *aoMessage.CastTo<CMemblock*>();
    lpoBlock = aoSession->GetWrite()->GetBlock();
    assert(lpoBlock != NULL);

    if (lpoMessage != NULL && lpoMessage->GetSize() != 0
            && lpoBlock->GetRead() != lpoMessage->GetRead())
    {
        if (lpoBlock->GetSpace() < lpoMessage->GetSize())
        {
            luSize = lpoBlock->GetCapacity() * 2;
            luSize = luSize > (lpoBlock->GetSize() + lpoMessage->GetSize()) ?
                luSize : lpoBlock->GetSize() + lpoMessage->GetSize();
            lpoBlock->Resize(luSize);
        }
        else if(lpoBlock->GetDirectSpace() < lpoMessage->GetSize())
        {
            lpoBlock->Shrink();
        }
        lpoBlock->Write(lpoMessage->GetRead(), lpoMessage->GetSize());
    }

    lpoEventDo = aoSession->GetCycle()->GetEventDo();
    if (lpoBlock->GetSize() != 0)
    {
        if (aoSession->GetService()->GetConfiguration()->IsDirectWrite())
        {
            liRet = lpoEventDo->Write(aoSession->GetWrite());
        }
        else
        {
            liRet = MINA_AGAIN;
        }
        if (liRet == MINA_AGAIN)
        {
            lpoEventDo->GetPoll()->Add(aoSession->GetWrite(), EPOLLOUT, 0);
        }
    }

}


void CFilterHead::SessionOpen(SessionType aoSession, CNextFilter *apoFilter)
{
    apoFilter->SessionOpen(aoSession);
}
void CFilterHead::SessionClose(SessionType aoSession, CNextFilter *apoFilter)
{
    apoFilter->SessionClose(aoSession);
}
void CFilterHead::SessionIdle(SessionType aoSession, uint32_t auStatus, CNextFilter *apoFilter)
{
    apoFilter->SessionIdle(aoSession, auStatus);
}
void CFilterHead::MessageRecived(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    apoFilter->MessageRecived(aoSession, aoMessage);
}
void CFilterHead::MessageFinished(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    apoFilter->MessageFinished(aoSession, aoMessage);
}
void CFilterHead::FilterClose(SessionType aoSession, CNextFilter *apoFilter)
{
    aoSession->GetFilterChain()->FireSessionClose(aoSession);
    //LOG_DEBUG("process_close locking");
}

void CFilterHead::FilterWrite(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    apoFilter->FilterWrite(aoSession, aoMessage);
}

MINA_NAMESPACE_END
