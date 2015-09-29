#include "Memblock.h"
#include "IoEvent.h"
#include "Log.h"
#include "Singleton.h"
#include "FilterChain.h"
#include "IoConnector.h"
#include "MessageTable.h"
#include "Process.h"
#include "RBTest.h"
#include "SKipListTest.h"

using namespace mina;
static void MinaReadIdle(CIoEvent *ev);
static void MinaWriteIdle(CIoEvent *ev);

class UDPTestHandler: public CIoHandler
{
public:
//    virtual ~IoHandler(void) {};

    virtual void SessionException(SessionType aoSession, CThrowable const& aoCause );
    virtual void SessionOpen(SessionType aoSession);
    virtual void SessionClose(SessionType aoSession);
    virtual void SessionIdle(SessionType aoSession, uint32_t status);
    virtual void MessageRecived(SessionType aoSession, CAny& aoMessage);
    virtual void MessageFinished(SessionType aoSession, CAny& aoMessage);
    virtual void FilterClose(SessionType aoSession);
    virtual void FilterWrite(SessionType aoSession, CAny& aoMessage);
};


int main(int args, char ** argv)
{
    RBTreeTest();

    SkipListTest();
/*
    CSockAddr loAddr;
    //loAddr.Set("10.3.3.60", 8888);
    loAddr.Set("224.0.0.88", 8888);
    CLog* lpoLog = Instance<CLog>();
    CProcessManager* lpoProcessor = Instance<CProcessManager>();
    CCycle* lpoCycle = new CCycle;
    CIoAcceptor loAcceptor(lpoCycle);

    lpoCycle->GetConfiguration()->SetMaxListener(10);
    lpoCycle->GetConfiguration()->SetMaxSession(1024);
    lpoCycle->GetConfiguration()->SetReadBufferSize(1024);
    lpoCycle->GetConfiguration()->SetWriteBufferSize(1024);
    lpoCycle->GetConfiguration()->SetReadIdle(1024);
    lpoCycle->GetConfiguration()->SetWriteIdle(1024);
    lpoCycle->GetConfiguration()->SetWriteTimeout(1024);
    lpoCycle->GetConfiguration()->SetAutoSuspendWrite(false);
    lpoCycle->GetConfiguration()->SetAutoResumeRead(false);
    lpoCycle->GetConfiguration()->SetMaxHint(1024);
    lpoCycle->GetConfiguration()->SetProcessors(10);
    lpoCycle->GetConfiguration()->SetThreadStackSize(1024);
    lpoCycle->GetConfiguration()->SetMaxDelay(600);

    lpoLog->Init(5, "log.log", 1024);
    lpoCycle->Init();
    loAcceptor.GetConfiguration()->Set(*lpoCycle->GetConfiguration());
    loAcceptor.GetConfiguration()->SetSessionPBlock(true);
    loAcceptor.SetHandler(new UDPTestHandler());
    lpoProcessor->BackupEnvirons(argv);
    lpoProcessor->SetTitle((char*)"Master");
    lpoProcessor->Bind(lpoCycle);
    loAcceptor.Join(loAddr);
    lpoProcessor->Loop();
  */
    return 0;
}

void UDPTestHandler::SessionException(SessionType aoSession, CThrowable const& aoCause )
{

    LOG_DEBUG("test session_Exception");
}
void UDPTestHandler::SessionOpen(SessionType aoSession)
{
    LOG_DEBUG("test session_open_handler");
}

void UDPTestHandler::SessionClose(SessionType aoSession)
{
    LOG_DEBUG("test session_close_handler, R:%x", aoSession->GetRead());
}

void UDPTestHandler::SessionIdle(SessionType aoSession, uint32_t status)
{
    if (status == MINA_READ_IDLE)
    {
        LOG_DEBUG("test session_idle_handler, R:%x, W:%x", aoSession->GetRead(), aoSession->GetWrite());
        MinaReadIdle(aoSession->GetRead());
    }
    else if (status == MINA_WRITE_IDLE)
    {
        LOG_DEBUG("test session_idle_handler, W:%x", aoSession->GetWrite());
        MinaWriteIdle(aoSession->GetWrite());
    }

}
void UDPTestHandler::MessageRecived(SessionType aoSession, CAny& aoMessage)
{
    CMemblock* reqblock = *aoMessage.CastTo<CMemblock*>();

    aoSession->GetDest()->Set("224.0.0.88", 8889);
    LOG_DEBUG("--------------------");
    //aoSession->SetDest(loAddr);
    reqblock->Write("HAHA", 5);
    aoSession->Write(aoMessage);
    reqblock->Reset();

}

void UDPTestHandler::MessageFinished(SessionType aoSession, CAny& aoMessage)
{
    LOG_DEBUG("test filter_finished_handler");

}
void UDPTestHandler::FilterClose(SessionType aoSession)
{
    LOG_DEBUG("test filter_close_handler");

}
void UDPTestHandler::FilterWrite(SessionType aoSession, CAny& aoMessage)
{
    LOG_DEBUG("test filter_write_handler");
}

static void MinaReadIdle(CIoEvent *ev)
{
    LOG_DEBUG("READ_IDLE:%d", ev);
}
static void MinaWriteIdle(CIoEvent *ev)
{
    LOG_DEBUG("WRITE_IDLE:%d", ev);
}
