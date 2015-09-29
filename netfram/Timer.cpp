#include "Timer.h"
#include "IoEvent.h"
MINA_NAMESPACE_START

CTimer::CTimer(void)
    : muSlot (0),
      muLock (0),
      mpoCacheTime (NULL),
      mpcFormatTime (NULL),
      muFormatLen (0)
{
    this->Init();
}

int64_t CTimer::GetNSec(void)
{
    return this->moTime.Nsec();
}
int64_t CTimer::GetUSec(void)
{
    return this->moTime.Usec();
}

int64_t CTimer::GetMSec(void)
{
    return this->moTime.Msec();
}

int64_t CTimer::GetSec(void)
{
    return this->moTime.Sec();
}

volatile char*
CTimer::GetFormatTime(void)
{
    return this->mpcFormatTime;
}

size_t
CTimer::GetFormatLen(void)
{
    return this->muFormatLen;
}

int
CTimer::Init(void)
{
    this->muFormatLen = sizeof("2013-07-10 01:30:07");
    this->muSlot = 0;
    this->mpoCacheTime = &moCacheTimes[0];
    this->Update();
    MINA_UNLOCK(&this->muLock);
    return 0;
}

bool CTimer::Empty(void)
{
    return this->moTimerHolder.Empty();
}

uint32_t
CTimer::Find(void)
{
  TSmartLock<CLock>(this->moLock);
  int timer;

  TimerIter loIter(&moTimerHolder);
  if (moTimerHolder.Empty())
  {
    return -1;
  }

  loIter.SeekToFirst();
  if (loIter.Valid())
  {
    timer = loIter.Key() - this->moTime.Msec();
    return (uint32_t)(timer > 0 ? timer: -1);
  }
  return -1;

}


void
CTimer::Expire(void)
{
    CIoEvent *lpoEvent;
    TimerIter loIter(&moTimerHolder);

    for (;;)
    {
        TSmartLock<CLock>(this->moLock);

        if (moTimerHolder.Empty())
        {
            break;
        }

        loIter.SeekToFirst();
        assert(loIter.Valid());
        int64_t now = this->moTime.Msec();
        int32_t liDiff = now - loIter.Key();

        if (liDiff >= 0)
        {
            lpoEvent = dynamic_cast<CIoEvent*>(loIter.Value());
            lpoEvent->PostTimeout(moTime);

            this->Delete(lpoEvent);

            continue;
        }
        break;
    }

}

void
CTimer::Delete(CIoEvent *apoEvent)
{
    TSmartLock<CLock>(this->moLock);
    assert(!this->moTimerHolder.Empty());
    moTimerHolder.Delete(apoEvent);
    apoEvent->TimerOn(false);
}

void CTimer::Add(CIoEvent *apoEvent, const CTimeValue& aoTimeout)
{
    this->Add(apoEvent, aoTimeout.Msec());
}

void
CTimer::Add(CIoEvent *apoEvent, uint64_t auTime)
{
    uint64_t luKey = 0;
    int liDiff = 0;

    if (auTime == 0 || auTime < MINA_TIMER_LAZY_DELAY)
    {
        return;
    }
    luKey = this->moTime.Msec() + auTime;
    if (apoEvent->TimerOn())
    {
        liDiff = (int) (luKey - apoEvent->mtKey);

        if (abs(liDiff) < MINA_TIMER_LAZY_DELAY)
            return;
        this->Delete(apoEvent);
    }
    apoEvent->mtKey = luKey;

    TSmartLock<CLock>(this->moLock);
    moTimerHolder.Insert(apoEvent);
    apoEvent->TimerOn(true);
}

void CTimer::Add(CTimerEvent *apoEvent, uint64_t auTime)
{
  this->Add(apoEvent->GetEvent(), auTime);
}
void
CTimer::Update()
{
    char *p0;
    struct tm *tm;
    time_t mlSec;
    uint32_t muMSec;
    CTimeValue *tp;
    struct timeval tv;
    struct timezone tz;


    if ( !MINA_TRYLOCK(&this->muLock) )
    {
        return;
    }

    gettimeofday(&tv, &tz);

    mlSec = tv.tv_sec;
    muMSec = tv.tv_usec / TIME_MSEC;

    this->moTime.Set(mlSec * TIME_SEC + tv.tv_usec * TIME_USEC, TIME_NSEC);

    tp = &this->moCacheTimes[muSlot];

    if (tp->Sec() == mlSec)
    {
        tp->Msec(muMSec);
        MINA_UNLOCK(&this->muLock);
        return;
    }

    if (muSlot == TIME_SLOTS - 1)
    {
        muSlot = 0;
    }
    else
    {
        muSlot++;
    }

    tp = &this->moCacheTimes[muSlot];
    tp->Sec(mlSec);
    tp->Msec(muMSec);

    p0 = &maacFormatTimes[muSlot][0];

    tm = gmtime(&mlSec);
    strftime(p0, FORM_TIME_LEN, "%Y-%m-%d %H:%M:%S", tm);

    this->mpoCacheTime = tp;
    this->mpcFormatTime = p0;

    MINA_UNLOCK(&this->muLock);


}
MINA_NAMESPACE_END
