#include <assert.h>
#include <stdio.h>
#include <error.h>
#include "Thread.h"
#include "Log.h"
#include "Singleton.h"

MINA_NAMESPACE_START


CThreadManager::CThreadManager(void)
    : muThreadCount (0)
    , muMaxThread (0)
    , muRegThreads (0)
{

}
uint32_t CThreadManager::GetMaxThread(void)
{
    return this->muMaxThread;
}
CThreadInfo* CThreadManager::GetThreadStats(void)
{
    return this->maoThreadStats;
}

int
CThreadManager::Init(size_t auN, size_t auSize, void* apfWorkerCycle(void *data) )
{
    int liRetCode;
    size_t luSize;

    this->muMaxThread = auN;
    liRetCode = pthread_attr_init(&this->moThrattr);
    assert(liRetCode == 0);
    luSize = auSize;

    if (luSize > 0 && luSize < PTHREAD_STACK_MIN * 2)
    {
        luSize = 0;
    }

    if (luSize != 0)
    {
        liRetCode = pthread_attr_setstacksize(&this->moThrattr, auSize);
        assert(liRetCode == 0);
    }

    if (auN != 0)
    {
        //liRetCode = mina_thread_key_create(
        /* whatever ngx_core_tls_key it is, it has been ignored. */

        for (size_t luN = 0; luN < auN; luN++)
        {
            if (this->muThreadCount == this->muMaxThread)
            {
                return MINA_ERROR;
            }
            if (this->Create((pthread_t *)&this->maoThreadStats[luN].muTid,
                        apfWorkerCycle,
                        (void *)&this->maoThreadStats[luN]) != 0)
            {
                assert(0);
                return MINA_ERROR;
            }
            this->muThreadCount++;
            LOG_DEBUG("Thread %u %d created", this->maoThreadStats[luN].muTid, this->maoThreadStats[luN].muState);
        }
    }

    return MINA_OK;
}


int
CThreadManager::SpawnN(size_t auN, void* apfWorkerCycle(void *data), void* apvArg)
{
    size_t luRegistered = this->muRegThreads;
    this->muRegThreads += auN;
    if (this->muRegThreads > sizeof(this->maoRegisteredTids) / sizeof(pthread_t))
    {
        return MINA_ERROR;
    }

    for (size_t luN = luRegistered; luN < auN; luN++)
    {
        if (this->Create(&this->maoRegisteredTids[luN], apfWorkerCycle, apvArg) != 0)
        {
            assert(0);
            return MINA_ERROR;
        }
    }

    return MINA_OK;
}


int
CThreadManager::Create(pthread_t *apuTid, void* (*Func)(void *apvArg), void *apvArg)
{
    return pthread_create(apuTid, &this->moThrattr, Func, apvArg);
}

void CThreadManager::WaitAll(void)
{
    void* lpvRes;
    for (size_t luN = 0; luN < this->muMaxThread; luN++)
    {
        pthread_join(this->maoThreadStats[luN].muTid, &lpvRes);
    }

    for (size_t luN = 0; luN < this->muRegThreads; luN++)
    {
        pthread_join(this->maoRegisteredTids[luN], &lpvRes);
    }
}

void CThreadManager::WakeupThread(void)
{
    uint32_t luN;
    int liRetCode;

    for (luN = 0; luN < this->GetMaxThread(); luN++)
    {
        if (this->maoThreadStats[luN].muState == MINA_THREAD_FREE)
        {
            liRetCode = this->maoThreadStats[luN].moCond.Signal();

            if (liRetCode != 0)
            {
                LOG_ERROR("pthread_cond_signal(moCond) fail");
            }

            return;
        }
    }
}

void CThreadManager::ExitAll(void)
{
    uint32_t luN;
    int liRetCode;
    bool lbFinish = true;

    do{
        lbFinish = true;
        for (luN = 0; luN < this->muMaxThread; luN++)
        {
            if (this->maoThreadStats[luN].muState == MINA_THREAD_FREE)
            {
                liRetCode = this->maoThreadStats[luN].moCond.Signal();
                if (liRetCode != 0)
                {
                    LOG_ERROR("pthread_cond_signal(moCond) fail");
                }

            }
            else if (this->maoThreadStats[luN].muState == MINA_THREAD_BUSY)
            {
                lbFinish = false;
            }
        }
    usleep(10);
    }while(!lbFinish);
}
MINA_NAMESPACE_END
