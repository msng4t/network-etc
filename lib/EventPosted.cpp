#include "EventPosted.h"
#include "SharedMutex.h"
#include "Thread.h"
#include "Singleton.h"
#include "Log.h"
#include "EventDo.h"
#include "Timer.h"
#include "Cycle.h"
#include "Channel.h"

MINA_NAMESPACE_START

CEventPosted::CEventPosted()
    : mpoCycle (NULL)
{
  moHead.Init();
  moAccept.Init();
}

void CEventPosted::Post(CIoEvent *apoEvent)
{
    TSmartLock<CLock> loGuaid(this->moLock);
    moHead.InsertBM(apoEvent);
}

void CEventPosted::PostAccept(CIoEvent *apoEvent)
{
    TSmartLock<CLock> loGuaid(this->moLock);
    moAccept.InsertBM(apoEvent);
}
bool CEventPosted::HasPosted( void )
{
    TSmartLock<CLock> loGuaid(this->moLock);
    return !moHead.Empty();
}

void CEventPosted::LockedDeletePosted(CIoEvent* apoEvent)
{
  apoEvent->Remove(apoEvent);
}

CLock& CEventPosted::GetLock(void)
{
    return this->moLock;
}

void CEventPosted::SetCycle( CCycle* apoCycle )
{
    this->mpoCycle = apoCycle;
}

void
CEventPosted::ProcessAccept( void )
{
//   LOG_DEBUG("In Process============================================");
    CIoEvent *lpoEvent;
    for (; ; )
    {
        lpoEvent = moAccept.Head()->Data<CIoEvent>();
        if (lpoEvent == NULL)
        {
            return;
        }
        {
          TSmartLock<CLock> loGuaid(this->moLock);
          this->LockedDeletePosted(lpoEvent);
        }
        if (lpoEvent->Accept())
        {
            this->mpoCycle->GetEventDo()->Accept(lpoEvent);
        }
    }
}

int
CEventPosted::ProcessPosted(void)
{
    CIoEvent *lpoEvent;
    int liRetCode = -1;

    for (; ; )
    {

        if (moHead.Empty())
        {
          break;
        }

        lpoEvent = moHead.Head()->Data<CIoEvent>();
        //lpoEvent = this->mpoPosted;
        for (; ; )
        {

            if (lpoEvent == NULL)
            {
                return MINA_OK;
            }
            LOG_DEBUG("IN ThreadProcess------lpoE:%d------ %p -------", (int)*lpoEvent->GetLock(), lpoEvent);
            if (ATOMIC_TRYLOCK(lpoEvent->GetLock()) == 0)
            {
                assert(lpoEvent != lpoEvent->Next()->Data<CIoEvent>());
                //LOG_DEBUG("%d, %d", lpoEvent, lpoEvent->next);
                lpoEvent = lpoEvent->Next()->Data<CIoEvent>();
                continue;
            }

            LOG_DEBUG("IN ThreadProcess=======poE:%d------ %p -------", (int)*lpoEvent->GetLock(), lpoEvent);
            if ( lpoEvent->GetLock() != lpoEvent->GetOwnLock())
            {
                if (*(lpoEvent->GetOwnLock()))
                {
                    ATOMIC_UNLOCK(lpoEvent->GetLock());
                    lpoEvent = lpoEvent->Next()->Data<CIoEvent>();
                    continue;
                }
                *(lpoEvent->GetOwnLock()) = 1;
            }

            this->LockedDeletePosted(lpoEvent);

            *(lpoEvent->GetLock()) = 1;
            this->moLock.Unlock();

            if (lpoEvent->Timeout())
            {
                lpoEvent->HandleTimeout();
            }

            else if (lpoEvent->Channel())
            {
                //now is just finish Wakeup
                MinaChannelHandler(lpoEvent);
            }
            else if (lpoEvent->Accept())
            {
                this->mpoCycle->GetEventDo()->Accept(lpoEvent);
            }
            else if ( !lpoEvent->GetSession()->ReadActive())
            {
                liRetCode = this->mpoCycle->GetEventDo()->Check(lpoEvent);
            }
            else if (!lpoEvent->Write())
            {
                liRetCode = this->mpoCycle->GetEventDo()->Read(lpoEvent);
                if (liRetCode != MINA_OK && liRetCode == MINA_AGAIN
                        && liRetCode != EINTR   && lpoEvent->TimerOn() )
                {
                    this->mpoCycle->GetTimer()->Delete(lpoEvent);
                }
            }
            else
            {
                liRetCode = this->mpoCycle->GetEventDo()->Write(lpoEvent);
            }

            this->moLock.Lock();

            if (*lpoEvent->GetLock() == 1)
            {
                *(lpoEvent->GetLock()) = 0;

                if (lpoEvent->GetLock() != lpoEvent->GetOwnLock())
                {
                    *(lpoEvent->GetOwnLock()) = 0;
                }
            }
            break;

        }
    }

    return MINA_OK;

}

void CEventPosted::PostEvent(CIoEvent *apoEvent, uint32_t auFlag)
{

    if ((auFlag & MINA_POST_THREAD_EVENTS) && !apoEvent->Accept())
    {
        apoEvent->Ready(1);
    }
    else
    {
        apoEvent->Ready(1);
    }

    if ((auFlag & MINA_POST_EVENTS) || !apoEvent->Accept())
    {
        if (apoEvent->Accept())
        {
            this->PostAccept(apoEvent);
        }
        else
        {
            this->Post(apoEvent);
        }
        LOG_DEBUG("POSTING++++++++++%p+++++++++++++++++++++++++++++", apoEvent);
    }
    else
    {
        LOG_DEBUG("not POSTING+++++++++++++++++++++++++++++++++++++++");
        this->mpoCycle->GetEventDo()->Accept(apoEvent);
    }
}

MINA_NAMESPACE_END
