#ifndef _CMINA_THREAD_H_INCLUDE_
#define _CMINA_THREAD_H_INCLUDE_

#include <pthread.h>
#include <inttypes.h>
#include "Define.h"
#include "Synch.h"

#define CMINA_MAX_THREADS 128

MINA_NAMESPACE_START
class CThreadInfo
{
public:
    pthread_t muTid;
    CCondition moCond;
    uint32_t muState;
};

#define MINA_THREAD_FREE 1
#define MINA_THREAD_BUSY 2
#define MINA_THREAD_EXIT 3
#define MINA_THREAD_DONE 4

class CThreadManager
{
public:
    CThreadManager(void);
    int Init(size_t auN, size_t muSize, void* apfWorkerCycle(void *data));
    int SpawnN(size_t auN, void* apfWorkerCycle(void *data), void* apvArg);
    int Create(pthread_t *mpuTid, void* (*Func)(void *mpvArg), void *mpvArg);
    uint32_t GetMaxThread(void);
    CThreadInfo* GetThreadStats(void);
    void WaitAll(void);
    void WakeupThread(void);
    void ExitAll(void);
private:
    uint32_t muThreadCount;
    uint32_t muMaxThread;
    uint32_t muRegThreads;
    CThreadInfo maoThreadStats[128];
    pthread_t maoRegisteredTids[32];
    pthread_attr_t moThrattr;
};

MINA_NAMESPACE_END

#endif
