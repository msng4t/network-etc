#ifndef _DAEMON_SERVICE_INCLUDE_
#define _DAEMON_SERVICE_INCLUDE_
#include <string>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include "Define.h"
#include "GetOpt.h"
#define GENHASXX(var)       \
    bool Has##var(void) { return (this->mi##var != 0);}
MINA_NAMESPACE_START
class CDaemonService
{
    struct CUOption
    {
        bool mbHited;
        bool mbGotValue;
        std::string moValue;
        std::string moLongOption;
        std::string moDescription;
    };

    typedef std::map<int, struct CUOption> UOptionMapType;
public:
    CDaemonService(void);
    ~CDaemonService(void);
    int Startup(void);
    int ReStart(void);
    void ShutDown(void);

    bool IsShutDown(void);
    void UpdateShutDown(bool abShutDown);
    char const* GetProcessName(void);
    ssize_t KillAll(int aiSignal);

    void PrintUsage(void);
    int GetOption(int argc, char *const *argv);
    int RunByDaemon(void);
    int SaveProcessID(void);
    void ClearProcessID(void);
    ssize_t KillAll(const char* apcProcessName, int aiSignal);
    int GetOptions(int argc, char *const *argv);
    GENHASXX( Version);
    GENHASXX( Help);
    GENHASXX( Kill);
    GENHASXX( Deamon);
private:
    int miArgc;
    char** mppcArgv;
    char macProcessName[1024];
    int miLogLevel;
    bool mbShutDown;
    int  miVersion;
    int  miHelp;
    int  miKill;
    int  miDeamon;

};
MINA_NAMESPACE_END
#endif
