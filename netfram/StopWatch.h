#ifndef MINA_WATCHER_H
#define MINA_WATCHER_H

#include "Timer.h"

MINA_NAMESPACE_START
#define NOW mpoTimer->GetMSec()
class StopWatch
{
public:
    int64_t elapsed;

    StopWatch (CTimer *apoTimer) { mpoTimer = apoTimer; Reset(); }

    void Reset() { elapsed = 0; }
    void Start() { started = NOW; }
    void Stop() { elapsed += NOW - started; }

private:
    CTimer* mpoTimer;
    int64_t started;
};

MINA_NAMESPACE_END
#endif

