#include "Process.h"
#include "Singleton.h"
#include "SocketOpt.h"
#include "Channel.h"

extern char **environ;

MINA_NAMESPACE_START

static void* WorkerThreadCycle(void *apvData);
void SignalHandler(int signo);

SSignalHandle CProcessManager::maoSignals[] = {
    {SIGHUP, "SIGHUP", "reload", SignalHandler},
    {SIGUSR1, "SIGUSR1", "reopen", SignalHandler},
    {SIGWINCH, "SIGWINCH", "", SignalHandler},
    {SIGTERM, "SIGTERM", "stop", SignalHandler},
    {SIGQUIT, "SIGQUIT", "quit", SignalHandler},
    {SIGUSR2, "SIGUSR2", "", SignalHandler},
    {SIGALRM, "SIGALRM", "", SignalHandler},
    {SIGINT, "SIGINT", "", SignalHandler},
    {SIGIO, "SIGIO", "", SignalHandler},
    {SIGCHLD, "SIGCHLD", "", SignalHandler},
    {SIGSYS, "SIGSYS,SIG_IGN", "", SIG_IGN},
    {SIGPIPE, "SIGPIPE,SIG_IGN", "", SIG_IGN},
    {0, NULL, "", NULL}
};

CProcessManager::CProcessManager(void)
    : mppcOSArgv (NULL)
    , mpcArgvLast (NULL)
    , mpacArgv (NULL)
    , miDeamonized (0)
    , miTerminate (0)
    , miQuit (0)
    , miExiting (0)
{

}

void SignalHandler(int aiSigno)
{
    const char* lpcAction;
    int liErr;
    int liIgnore;
    SSignalHandle* loSignals;
    CProcessManager* lpoManager;

    lpoManager = Instance<CProcessManager>();
    LOG_ERROR("loSignals received:%d _______________\n", aiSigno);
    liIgnore = 0;
    liErr = errno;

    for (loSignals = lpoManager->maoSignals; loSignals->miSigno != 0; loSignals++)
    {
        if (loSignals->miSigno == aiSigno)
        {
            break;
        }
    }

    lpcAction = "";

    switch (0)
    {
        switch (aiSigno)
        {
            case SIGQUIT:
                lpoManager->miQuit = 1;
                lpcAction = ",shuting down";
                break;
            case SIGTERM:
                lpoManager->miTerminate = 1;
                lpcAction = ",exiting";
                break;
            case SIGINT:
                lpoManager->miTerminate = 1;
                break;
            case SIGALRM:
                lpoManager->miTerminate = 1;
                break;
            case SIGCHLD:
                lpoManager->miReap = 1;
                break;
        }

    }

    LOG_ERROR("signal %d (%liNum) received %liNum", aiSigno, loSignals->mpcSigName, lpcAction);

    if (liIgnore)
    {
        LOG_ERROR("the changing binary signal is liIgnored");
    }

    if (aiSigno == SIGCHLD)
    {
        lpoManager->GetExitStatus();
    }

    errno = liErr;
}

int CProcessManager::InitSignals(void)
{
    SSignalHandle *loSignals;
    struct sigaction loSaction;

    for (loSignals = this->maoSignals; loSignals->miSigno != 0; loSignals++)
    {
        memset(&loSaction, 0, sizeof(struct sigaction));
        loSaction.sa_handler = loSignals->mpfHandler;
        loSaction.sa_flags = 0;
        sigemptyset(&loSaction.sa_mask);

        if (sigaction(loSignals->miSigno, &loSaction, NULL) == -1)
        {
            LOG_ERROR("sigaction(%liNum) failed", loSignals->mpcSigName);
            return MINA_ERROR;
        }
    }
    return MINA_OK;
}


int
CProcessManager::Kill(char* apcName, pid_t luPid)
{
    SSignalHandle *loSignals;

    for (loSignals = this->maoSignals; loSignals->miSigno != 0; loSignals++)
    {
        if(strcasecmp(apcName, loSignals->mpcName) == 0)
        {
            if (kill(luPid, loSignals->miSigno) != -1)
            {
                return MINA_OK;
            }

            LOG_ERROR("kill(%d, %d) failed", luPid, loSignals->miSigno);
        }
    }
    return 1;
}


void CProcessManager::GetExitStatus(void)
{
    int liStatus;
    char *lpcProcess;
    pid_t luPid;
    int liErr;
    bool lbOnce = false;

    for ( ;; )
    {
        luPid = waitpid(-1, &liStatus, WNOHANG);
        if (luPid == 0){
            return;
        }

        if (luPid == -1)
        {
            liErr = errno;

            if (liErr == EINTR) {
                continue;
            }

            if (liErr == ECHILD && lbOnce){
                return;
            }

            if (liErr == ECHILD)
            {
                LOG_ERROR("waitpid() failed");
                return;
            }
        }

        lbOnce = true;
        lpcProcess = (char*)"unknown lpcProcess";

        if (WTERMSIG(liStatus))
        {
#ifdef WCOREDUMP
            LOG_ERROR("%li %d exited on signal %d %liNum",
                    lpcProcess,
                    luPid,
                    WTERMSIG(liStatus),
                    WCOREDUMP(liStatus) ? "(core dumped)": ""
                    );
#else
            LOG_ERROR("%li %d exited on signal %d",
                    lpcProcess,
                    luPid,
                    WRERMSIG(liStatus)
                    );
#endif
        }
        else
        {
            LOG_ERROR("%li %d exited with code %d", lpcProcess, luPid, WEXITSTATUS(liStatus));
        }

    }
}




void CProcessManager::Bind(CCycle* apoCycle)
{

    this->mpoCylce = apoCycle;
    this->moThreadManager.Init(this->mpoCylce->GetConfiguration()->GetProcessors(),
            this->mpoCylce->GetConfiguration()->GetThreadStackSize(),
            WorkerThreadCycle);

}


void CProcessManager::Loop(void)
{
    uint32_t liN;
    CSession *lpoSession;

    this->mpoCylce->GetEventDo()->EnableAccept();
    for (; ; )
    {
        if (this->miExiting)
        {
            lpoSession = this->mpoCylce->GetSessions();

            for (liN = 0; liN < this->mpoCylce->GetSessionN(); liN++)
            {
                if (lpoSession[liN].GetFd() != -1 && lpoSession[liN].Idle())
                {
                    lpoSession[liN].Closed(true);
                }
            }

            if (this->mpoCylce->GetTimer()->Empty())
            {
                LOG_ERROR("exiting");
            }
        }

        if (this->miQuit)
        {
            LOG_FLUSH();
            break;
        }
        this->mpoCylce->GetEventDo()->Process(&this->moThreadManager);

    }
    this->moThreadManager.WaitAll();
}


static void* 
WorkerThreadCycle(void *apvData)
{
    sigset_t loSigSet;
    int liError;
    CProcessManager *lpoProcessor = NULL;
    CThreadInfo *lpoThreadStat = (CThreadInfo*)apvData;

    lpoProcessor = Instance<CProcessManager>();

    sigemptyset(&loSigSet);
    sigaddset(&loSigSet, SIGHUP);
    sigaddset(&loSigSet, SIGUSR1);
    sigaddset(&loSigSet, SIGUSR2);

    liError = (sigprocmask(SIG_BLOCK, &loSigSet, NULL) == -1) ? errno : 0;

    if (liError)
    {
        LOG_ERROR("thread sigprocmask(SIG_BLOCK) failed");
        return (void*) 1;
    }



    for (; ; )
    {
        lpoThreadStat->muState = MINA_THREAD_FREE;
        LOG_DEBUG("Waiting for wakeuping.........%d ....", &lpoThreadStat->moCond);
        if (lpoThreadStat->moCond.Wait(lpoProcessor->GetCycle()->GetPostDo()->GetLock()) == MINA_ERROR)
        {
            return (void*) 1;
            //LOG_DEBUG("Wakeuping ERROR===========================");
        }

        if (lpoProcessor->miTerminate || lpoProcessor->miQuit == 1)
        {
            lpoThreadStat->muState = MINA_THREAD_EXIT;
            //LOG_DEBUG("terminate unlocking ");
            lpoProcessor->GetCycle()->GetPostDo()->GetLock().Unlock();
            LOG_DEBUG("thread %d is done", pthread_self());

            return (void*) 0;
        }

        lpoThreadStat->muState = MINA_THREAD_BUSY;
        //LOG_DEBUG("Wakeup, be busy===========================");
        if (lpoProcessor->GetCycle()->GetPostDo()->ProcessPosted() != MINA_OK)
        {
            return (void*) 0;
        }

        if (lpoProcessor->GetCycle()->GetPostDo()->ProcessPosted() != MINA_OK)
        {
            return (void*) 0;
        }

    }
}



int
CProcessManager::BackupEnvirons(char* apacArgv[])
{
    char      *lpcDest;
    size_t       luSize;
    uint32_t   luN;

    luSize = 0;
    for (luN = 0; environ[luN]; luN++)
    {
        luSize += strlen(environ[luN]) + 1;
    }

    lpcDest = new char[luSize];
    assert(lpcDest != NULL);
    this->mpacArgv = apacArgv;
    this->mpcArgvLast = this->mpacArgv[0];

    /* set mpcArgvLast to the position after last environs */
    for (luN = 0; this->mpacArgv[luN]; luN++)
    {
        if (this->mpcArgvLast == this->mpacArgv[luN])
        {
            this->mpcArgvLast = this->mpacArgv[luN]+ strlen(this->mpacArgv[luN]) + 1;
        }
    }

    /*  backup environ[luN] to the newly malloced memory */
    for (luN = 0; environ[luN]; luN++)
    {
        if (this->mpcArgvLast == environ[luN])
        {
            luSize = strlen(environ[luN]) + 1;
            this->mpcArgvLast = environ[luN]+ luSize;
            strncpy(lpcDest, environ[luN], luSize);
            environ[luN] = (char *) lpcDest;
            lpcDest += luSize;
        }
    }

    this->mpcArgvLast--;

    return 0;
}

void
CProcessManager::SetTitle(char *apcTitle)
{
    char*  lpcDest;
    size_t luLen;
    const char* lpcTitle = "mina: ";

    this->mpacArgv[1] = NULL;

    luLen = strlen(lpcTitle);
    memcpy(this->mpacArgv[0], lpcTitle, luLen);
    lpcDest = this->mpacArgv[0] + luLen;

    luLen = strlen(apcTitle);
    memcpy(lpcDest, apcTitle, luLen);
    lpcDest = lpcDest + luLen;

    if (this->mpcArgvLast - lpcDest)
    {
        memset(lpcDest, 0, this->mpcArgvLast - lpcDest);
    }

}
CCycle* CProcessManager::GetCycle( void )
{
    return this->mpoCylce;
}

MINA_NAMESPACE_END
