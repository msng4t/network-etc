#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <assert.h>
#include <stdio.h>
#include <error.h>
#include <signal.h>
#include "Define.h"
#include "Queue.h"
#include "Synch.h"

class CWorkable
{
public:
    virtual ~CWorkable(void){}
    virtual void Execute(void){}
};

struct WorkNode: public CQueueNode
{
    CWorkable* mpoInfo;
};

class ThreadPool
{
public:
	virtual ~ThreadPool();
    ThreadPool(int numThread);

	virtual void		Start();
	virtual void		Stop();

	virtual void		Execute(CWorkable *apoNode) = 0;

	int					NumPending() { return numPending; }
	int					NumActive() { return numActive; }
	int					NumTotal() { return numPending + numActive; /* atomicity bug */ }
    static void* ThreadFunc(void* param);

protected:
	CQueueNode		   moNodes;
	int					numPending;
	int					numActive;
	int					numThread;
	bool				running;
	void			    ThreadCycle();
	pthread_t*		    threads;
	pthread_mutex_t	    mutex;
	pthread_cond_t	    cond;
};


#endif
