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
#include "HttpMessage.h"

#include <stdio.h>

using namespace mina;

class TestTimer: public CTimerEvent
{
  public:
    CMemblock moBlock;

    void Init()
    {
      CHttpRequest loRequest;
//      loRequest.SetMethod("PUT");
//      loRequest.SetRequestURI("/v2/keys/message");
      loRequest.SetMethod("GET");
      loRequest.SetRequestURI("/v2/keys/foo_dir?wait=true&recursive=true&waitIndex=305");
      loRequest.SetRequestURI("/");
      loRequest.SetHttpVersion("1.1");
      loRequest.SetHeader("User-Agent", "User-Agent: curl/7.29.0");
      loRequest.SetHeader("Host", "121.14.39.195:18080");
      loRequest.SetHeader("Accept", "*/*");
//      loRequest.SetHeader("Content-Length", "9");
//      loRequest.SetHeader("Content-Type", "application/x-www-form-urlencoded");
//      loRequest.SetBody("value=bar");
//      loRequest.SetBody("");
      moBlock.Resize(1024);
      loRequest.EncodeMessage(&moBlock);
      printf("%s\n", moBlock.GetRead());
      printf("_____\n");
    }

    int HandleTimeout( void )
    {
      CSession* lpoSession = NULL;
      if (GetCycle()->GetSessionManager()->Find(lpoSession, "127.0.0.1", 4001))
      {
        CAny loRequest(&moBlock); 
        lpoSession->Write(loRequest);
        printf("HAHA Test.\n");
      }
      else
      {
        printf("HAHA Test failure.\n");
      }

      GetCycle()->GetTimer()->Add(this, 100 * 1000);
      return 0;
    }
    ~TestTimer()
    {

    }
};

int main(int args, char ** argv)
{

    CSockAddr loAddr;
    loAddr.Set("127.0.0.1", 4001);
    CLog* lpoLog = Instance<CLog>();
    CProcessManager* lpoProcessor = Instance<CProcessManager>();
    CCycle* lpoCycle = GetCycle();
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
//    loConnector.Connect("Test", loAddr1, 3);
    TestTimer tt;
    tt.Init();
    lpoCycle->GetTimer()->Add(&tt, 2 * 1000);
    lpoProcessor->Loop();
    delete lpoCycle;

    return 0;
}

