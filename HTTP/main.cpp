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
    loAddr.Set("0.0.0.0", 18082);
    CSockAddr loAddr1;
//    loAddr1.Set("192.168.1.101", 18089);
    loAddr1.Set("10.3.3.108", 18080);
    CLog* lpoLog = Instance<CLog>();
    CProcessManager* lpoProcessor = Instance<CProcessManager>();
    CCycle* lpoCycle = new CCycle;
    CIoAcceptor loAcceptor(lpoCycle);
    CIoConnector loConnector(lpoCycle);
    CIoAcceptor loAcceptor2(lpoCycle);

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
    lpoCycle->SetAcceptor(&loAcceptor);
    lpoCycle->SetAcceptor(&loAcceptor2);
    loAcceptor.SetHandler(new CHttpHandler());
    loAcceptor2.SetHandler(new CHttpHandler());
    loConnector.SetHandler(new CHttpHandler());

    lpoProcessor->BackupEnvirons(argv);
//    lpoProcessor->SetTitle((char*)"Master");
    lpoProcessor->Bind(lpoCycle);
    if (loAcceptor.Bind("FF1", "0.0.0.0", 18080) != MINA_OK)
    {
        printf("Bind failed(%s)", strerror(errno));
        return -1;
    }
    if (loAcceptor2.Bind("FF2", "0.0.0.0", 18082) != MINA_OK)
    {
        printf("Bind failed(%s)", strerror(errno));
        return -1;
    }
//    loConnector.Connect("Test", loAddr1, 3);
    lpoProcessor->Loop();
    delete lpoCycle;

    return 0;
}

