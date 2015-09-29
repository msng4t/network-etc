#ifndef _CMINA_PROCESS_H_INCLUDE_
#define _CMINA_PROCESS_H_INCLUDE_
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Define.h"
#include "Cycle.h"
#include "Log.h"
#include "Channel.h"
#include "Timer.h"
#include "IoConnector.h"
#include "Thread.h"

#define IVALID_PID -1 
#define MINA_MAX_PROCESSES 1024
#define MINA_SINGLE 0
#define MINA_MASTER 1
#define MINA_SIGNALER 2
#define MINA_WORKER 3
#define MINA_HELPER 4

MINA_NAMESPACE_START
typedef void (*WorkingCycle)(CCycle *cycle, void* mpvData);
typedef void* WorkerFunc(void* apvData);

void MinaChannelHandler(CIoEvent *apoEvent);


typedef struct 
{
    int miSigno;
    char const* mpcSigName;
    char const* mpcName;
    void (*mpfHandler)(int aiSigno);
}SSignalHandle;

void SignalHandler(int aiSigno);

class CProcessManager
{
public:
    CProcessManager(void);
    int InitSignals(void);
    int Kill(char* mpcName, pid_t muPid);
    void GetExitStatus(void);

    void Bind(CCycle* apoCycle);
    void Loop(void);
    int BackupEnvirons(char* apacArgv[]);
    void SetTitle(char *apcTitle);
    void SetConnector(CIoConnector *apoConnector);
    void SetConfig(CConfiguration* apoConfig);
    CCycle* GetCycle( void );

private:

    char** mppcOSArgv;
    CThreadManager moThreadManager;
    char *mpcArgvLast;
    char** mpacArgv;
    CCycle* mpoCylce;
public:
    static SSignalHandle maoSignals[];
    sig_atomic_t miDeamonized;
    sig_atomic_t miTerminate;
    sig_atomic_t miQuit;
    sig_atomic_t miExiting;
    sig_atomic_t miReap;

};

MINA_NAMESPACE_END
#endif
