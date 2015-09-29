#include "ThreadPool.h"

void ThreadPool::ThreadCycle()
{
    WorkNode* lpoNode;

    while (running)
    {
        pthread_mutex_lock(&mutex);
        numActive--;

wait:
        while (numPending == 0 && running)
            pthread_cond_wait(&cond, &mutex);

        if (!running)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        if (moNodes.Empty())
            goto wait;

        lpoNode = moNodes.Next()->Data<WorkNode>();
        lpoNode->Remove(lpoNode);
        numPending--;
        numActive++;

        pthread_mutex_unlock(&mutex);

        lpoNode->mpoInfo->Execute();
        delete lpoNode;
    }
}

ThreadPool::ThreadPool(int numThread) :
    numThread(numThread)
{
    numPending = 0;
    numActive = 0;
    running = false;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    threads = new pthread_t[numThread];
}

ThreadPool::~ThreadPool()
{
    Stop();
    delete[] threads;
}

void ThreadPool::Execute(CWorkable* apoNode)
{
    pthread_mutex_lock(&mutex);

    WorkNode* lpoNode = new WorkNode;
    assert(lpoNode != NULL);
    lpoNode->mpoInfo = apoNode;
    moNodes.InsertBM(lpoNode);
    numPending++;

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);
}

void* ThreadPool::ThreadFunc(void* param)
{
    sigset_t        sigmask;
    ThreadPool* tp = (ThreadPool*) param;

    sigfillset(&sigmask);
    pthread_sigmask(SIG_SETMASK, &sigmask, NULL);
    tp->ThreadCycle();

    return NULL;
}


void ThreadPool::Start()
{
    int i;

    if (running)
        return;

    running = true;
    numActive = numThread;

    for (i = 0; i < numThread; i++)
    {
        pthread_create(&threads[i], NULL, ThreadFunc, this);
    }
}

void ThreadPool::Stop()
{
    int i;

    if (!running)
        return;

    running = false;

    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for (i = 0; i < numThread; i++)
    {
        pthread_join(threads[i], NULL);
        pthread_detach(threads[i]);
    }

    numActive = 0;
}

