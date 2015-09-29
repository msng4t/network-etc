#ifndef _TIMER_INCLUDE_
#define _TIMER_INCLUDE_
#include <time.h>
#include <inttypes.h>
#include "TimeValue.h"
#include "RBTree.h"
#include "Synch.h"
#include "SharedMutex.h"
#include "TSKipList.h"
#include "Any.h"

MINA_NAMESPACE_START

#define MINA_TIMER_INFINITE -1
#define TIME_SLOTS 12
#define FORM_TIME_LEN sizeof("2013-07-10 01:30:07")
#define MINA_TIMER_LAZY_DELAY 1
class CIoEvent;
class CTimerEvent;
class CTimer
{
public:
    CTimer(void);
    int64_t GetNSec(void);
    int64_t GetUSec(void);
    int64_t GetMSec(void);
    int64_t GetSec(void);
    volatile char* GetFormatTime(void);
    size_t GetFormatLen(void);
    int Init(void);
    uint32_t Find(void);
    void Expire(void);

    void Delete(CIoEvent *apoEvent);
    void Add(CIoEvent *apoEvent,
            const void* apvAct,
            CTimeValue& aoTimerValue,
            CTimeValue& aoInterval);
    void Add(CIoEvent *apoEvent, uint64_t auTime);
    void Add(CIoEvent *apoEvent, const CTimeValue& aoTimeout);
    void Add(CTimerEvent *apoEvent, uint64_t auTime);

    bool Empty(void);

    void Update(void);

private:
    uint32_t muSlot;
    volatile uint64_t muLock;
    CTimeValue moTime;
    volatile CTimeValue *mpoCacheTime;
    volatile char* mpcFormatTime;

    size_t muFormatLen;
    char maacFormatTimes[TIME_SLOTS][FORM_TIME_LEN];
    CTimeValue moCacheTimes[TIME_SLOTS];

#ifndef SKIPTIMER
    CRBTree<uint64_t> moTimerHolder;
    typedef CRBTree<uint64_t> ::Iterator TimerIter;
#else
    CSkipList<uint64_t, CIoEvent*> moTimerHolder;
    typedef CSkipList<uint64_t, CIoEvent*> ::Iterator TimerIter;
#endif
    CLock moLock;



};

MINA_NAMESPACE_END
#endif
