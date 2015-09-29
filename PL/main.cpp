#include <stdlib.h>
#include "Timer.h"
#include "IoEvent.h"
#include "Configuration.h"
#include "Process.h"
#include "StopWatch.h"
#include "Table.h"
#include "Transaction.h"
#include "Database.h"
#include "Config.h"
#include "PLHandler.h"
#include "ReplicatedLog.h"

#include <stdio.h>

using namespace mina;

int main(int args, char ** argv)
{

    DatabaseConfig      loDbConfig;
    const char*         lpcGroup = NULL;
    CSockAddr           loMAddr;
    int                 liPort = 0;
    CLog*               lpoLog = Instance<CLog>();
    CProcessManager*    lpoProcessor = Instance<CProcessManager>();
    CCycle*             lpoCycle = new CCycle;
    CIoAcceptor         loAcceptor(lpoCycle);
    int nodeID;

    lpoCycle->GetConfiguration()->SetMaxListener(10);
    lpoCycle->GetConfiguration()->SetMaxSession(1024);
    lpoCycle->GetConfiguration()->SetReadBufferSize(1024);
    lpoCycle->GetConfiguration()->SetWriteBufferSize(1024);
    lpoCycle->GetConfiguration()->SetReadIdle(300);
    lpoCycle->GetConfiguration()->SetWriteIdle(300);
    lpoCycle->GetConfiguration()->SetWriteTimeout(1024);
    lpoCycle->GetConfiguration()->SetAutoSuspendWrite(false);
    lpoCycle->GetConfiguration()->SetAutoResumeRead(false);
    lpoCycle->GetConfiguration()->SetMaxHint(1024);
    lpoCycle->GetConfiguration()->SetProcessors(1);
    lpoCycle->GetConfiguration()->SetThreadStackSize(1024);
    lpoCycle->GetConfiguration()->SetMaxDelay(600);

    lpoLog->Init(5, "log.log", 1024, lpoCycle->GetTimer());

    lpoCycle->Init();

    Config::Init("../Config/paxos.ini");
    lpoProcessor->BackupEnvirons(argv);
    lpoProcessor->SetTitle((char*)"Master");
    loAcceptor.GetConfiguration()->Set(*lpoCycle->GetConfiguration());
    loAcceptor.SetHandler(new CPLHandler(ReplicatedLog::Get()));

    if (!database.Init(loDbConfig))
    {
        LOG_ERROR("Cannot Open init database.");
    }

    if (!RCONF->Init())
    {
        LOG_ERROR("Cannot Init ReplicatedLog");
    }
    lpoProcessor->Bind(lpoCycle);
    lpcGroup = Config::GetValue("group", "224.0.0.88");
    liPort = Config::GetIntValue("mport", 8888);
    nodeID = RCONF->GetNodeID();
    loMAddr = RCONF->GetAddr(RCONF->GetNodeID());
    if (loAcceptor.BindUDP(loMAddr) != MINA_OK)
    {
        printf("Bind failed(%s)\n", strerror(errno));
        return -1;
    }
    lpoProcessor->Loop();

    return 0;
}

