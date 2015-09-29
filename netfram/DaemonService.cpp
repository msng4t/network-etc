#include <stdlib.h>
#include "DaemonService.h"
#include "Log.h"
#include "Singleton.h"
MINA_NAMESPACE_START

CDaemonService::CDaemonService(void)
{

}
CDaemonService::~CDaemonService(void)
{

}
int
CDaemonService::Startup(void)
{
    return 0;
}
int
CDaemonService::ReStart(void)
{
    return -1;
}

void
CDaemonService::ShutDown(void)
{
    this->UpdateShutDown(true);
}

void
CDaemonService::UpdateShutDown(bool abShutDown)
{
    this->mbShutDown = abShutDown;
}
bool
CDaemonService::IsShutDown(void)
{
    return this->mbShutDown;
}
char const*
CDaemonService::GetProcessName(void)
{
    return this->macProcessName;
}
ssize_t
CDaemonService::KillAll(int aiSignal)
{
    return this->KillAll(this->macProcessName, aiSignal);
}

void
CDaemonService::PrintUsage(void)
{
    fprintf(stderr, "-k kill the runing process\n");
    fprintf(stderr, "-v show the current version\n");
    fprintf(stderr, "-h show the help information\n");
}

int
CDaemonService::RunByDaemon(void)
{
    int liFd;

    switch (fork())
    {
        case -1:
            return MINA_ERROR;
        case 0:
            break;
        default:
            exit(0);
    }

    if (setsid() == -1)
    {
        LOG_ERROR("setsid() failed");
        return MINA_ERROR;
    }

    umask(0);

    liFd = open("/dev/null", O_RDWR);

    if (liFd == -1)
    {
        LOG_ERROR("open(\"/dev/null\") failed");
        return MINA_ERROR;
    }

    if (dup2(liFd, STDIN_FILENO) == -1)
    {
        LOG_ERROR("dup2(liFd, STDIN_FILENO) failed");
        return MINA_ERROR;
    }

    if (dup2(liFd, STDOUT_FILENO) == -1)
    {
        LOG_ERROR("dup2(liFd, STDOUT_FILENO) failed");
        return MINA_ERROR;
    }

#if 0
    if (dup2(liFd, STDERR_FILENO) == -1)
    {
        LOG_ERROR("dup2(liFd, STDOUT_FILENO) failed");
        return MINA_ERROR;
    }
#endif

    if (liFd > STDERR_FILENO)
    {
        if(close(liFd) == -1)
        {
            LOG_ERROR("close(liFd) failed");
            return MINA_ERROR;
        }
    }
    return MINA_OK;
}

int
CDaemonService::SaveProcessID(void)
{
    size_t liLen;
    size_t luSize;
    int liFd;
    char lacpid[MAX_NAME_LEN];
    snprintf(lacpid, MAX_NAME_LEN, "%s.pid", this->macProcessName);

    if ( (liFd = open(lacpid, O_WRONLY | O_CREAT |O_TRUNC, S_IREAD|S_IWRITE)) == -1)
    {
        LOG_ERROR("create pid file error!%d", errno);
        return MINA_ERROR;
    }

    if (snprintf(lacpid, sizeof(lacpid), "%d", getpid()) < 0)
    {
        LOG_ERROR("snprintf pid fail!");
        return MINA_ERROR;
    }
    LOG_ERROR("before snprintf pid");
    luSize = strlen(lacpid);

    if ((liLen = write(liFd, lacpid, luSize)) != luSize)
    {
        LOG_ERROR("write pid file failed!");
        close(liFd);
        return MINA_ERROR;
    }

    close(liFd);
    return MINA_OK;

}

void
CDaemonService::ClearProcessID(void)
{
    char lacpid[MAX_NAME_LEN];
    snprintf(lacpid, MAX_NAME_LEN, "%s.pid", this->macProcessName);
    unlink(lacpid);
}

ssize_t
CDaemonService::KillAll(const char* apcProcessName, int aiSignal)
{
    int luSize;
    int liFd;
    int liPid;
    char lacpid[MAX_NAME_LEN];

    snprintf(lacpid, MAX_NAME_LEN, "%s.pid", apcProcessName);
    if ( (liFd = open(lacpid, O_RDONLY)) == -1)
    {
        fprintf(stderr, "\nRead pid file(%s.pid) error, %s\n", apcProcessName, strerror(errno));
        return MINA_ERROR;
    }
    memset(lacpid, 0, sizeof(lacpid));
    luSize = read(liFd, lacpid, sizeof("-9223372036854775808") - 1);

    close(liFd);
    if (luSize < 0)
    {
        return 1;
    }

    while (luSize-- && (lacpid[luSize] == 10 || lacpid[luSize] == 13) )
    {
        lacpid[luSize] = '\0';
    }

    liPid = atoi(lacpid);

    if (liPid < 0)
    {
        return 1;
    }

    if (kill(liPid, aiSignal) == -1)
    {
        return 1;
    }

    return 0;


}

int
CDaemonService::GetOptions(int argc, char *const *argv)
{
    u_char     *p;
    int         i;

    char const* lpcProcname = strrchr(argv[0],'/' );
    if (lpcProcname == NULL)
        strncpy(this->macProcessName, argv[0], sizeof(this->macProcessName));
    else
        strncpy(this->macProcessName, ++lpcProcname, sizeof(this->macProcessName));
    this->miVersion = 0;
    this->miHelp = 0;
    this->miKill = 0;


    for (i = 1; i < argc; i++) {
        fprintf(stderr, "op: %s\n", argv[i]);

        p = (u_char *) argv[i];

        if (*p++ != '-') {
            fprintf(stderr, "invalid option: \"%s\"", argv[i]);
            return MINA_ERROR;
        }

        while (*p) {

            switch (*p++) {

            case '?':
            case 'h':
                this->miVersion = 1;
                this->miHelp = 1;
                break;

            case 'k':
                this->miKill = 1;
                break;

            case 'v':
                this->miVersion = 1;
                break;

            case 'd':
                this->miDeamon = 1;
                break;

            case 'V':
                this->miVersion = 1;
                break;

            default:
                fprintf(stderr, "invalid option: \"%c\"", *(p - 1));
                return MINA_ERROR;
            }
        }
    }

    return MINA_OK;
}
MINA_NAMESPACE_END
