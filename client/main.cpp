#include <stdlib.h>
#include "Timer.h"
#include "IoEvent.h"
#include "Configuration.h"
#include "Process.h"
#include "StopWatch.h"
#include "HttpHandler.h"
#include "Table.h"
#include "Transaction.h"
#include "Database.h"

#include <stdio.h>

using namespace mina;

int main(int args, char ** argv)
{

    CSockAddr loAddr;
    loAddr.Set("172.19.35.165", 18082);
    CLog* lpoLog = Instance<CLog>();
    CProcessManager* lpoProcessor = Instance<CProcessManager>();
    CCycle* lpoCycle = new CCycle;
    CIoConnector loConnector(lpoCycle);

    lpoCycle->GetConfiguration()->SetMaxListener(10);
    lpoCycle->GetConfiguration()->SetMaxSession(1024);
    lpoCycle->GetConfiguration()->SetReadBufferSize(1024);
    lpoCycle->GetConfiguration()->SetWriteBufferSize(1024);
    lpoCycle->GetConfiguration()->SetReadIdle(5);
    lpoCycle->GetConfiguration()->SetWriteIdle(5);
    lpoCycle->GetConfiguration()->SetWriteTimeout(1024);
    lpoCycle->GetConfiguration()->SetAutoSuspendWrite(false);
    lpoCycle->GetConfiguration()->SetAutoResumeRead(false);
    lpoCycle->GetConfiguration()->SetMaxHint(1024);
    lpoCycle->GetConfiguration()->SetProcessors(1);
    lpoCycle->GetConfiguration()->SetThreadStackSize(1024);
    lpoCycle->GetConfiguration()->SetMaxDelay(600);

    lpoLog->Init(5, "log.log", 1024);
    lpoCycle->Init();
    loConnector.SetHandler(new CHttpHandler());

    lpoProcessor->BackupEnvirons(argv);
    lpoProcessor->SetTitle((char*)"Master");
    lpoProcessor->Bind(lpoCycle);
    loConnector.Connect("Test", loAddr, 3);
    lpoProcessor->Loop();

    return 0;
}

