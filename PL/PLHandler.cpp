#include "PLHandler.h"
#include "Memblock.h"
#include "IoEvent.h"
#include "Log.h"
#include "Singleton.h"
#include "FilterChain.h"
#include "IoConnector.h"
#include "SessionManager.h"
#include "MessageTable.h"

MINA_NAMESPACE_START

static void MinaReadIdle(CIoEvent *ev);
static void MinaWriteIdle(CIoEvent *ev);

void CPLHandler::SessionException(SessionType aoSession, CThrowable const& aoCause )
{

    LOG_DEBUG("test session_Exception");
}
void CPLHandler::SessionOpen(SessionType aoSession)
{
    ReplicatedLog::Get()->Init(aoSession);
}

void CPLHandler::SessionClose(SessionType aoSession)
{
    LOG_DEBUG("test session_close_handler, R:%x", aoSession->GetRead());
}

void CPLHandler::SessionIdle(SessionType aoSession, uint32_t status)
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
void CPLHandler::MessageRecived(SessionType aoSession, CAny& aoMessage)
{
    int liRet = -1;
    CMemblock* lpoBlock = *aoMessage.CastTo<CMemblock*>();
    LOG_DEBUG("%d recived:%s.", lpoBlock->GetSize(), lpoBlock->GetRead());
    ReplicatedLog::Get()->GetLeaser()->OnRead();
    lpoBlock->Reset();

}

void CPLHandler::MessageFinished(SessionType aoSession, CAny& aoMessage)
{
    LOG_DEBUG("test filter_finished_handler");

}
void CPLHandler::FilterClose(SessionType aoSession)
{
    LOG_DEBUG("test filter_close_handler");

}
void CPLHandler::FilterWrite(SessionType aoSession, CAny& aoMessage)
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

CPLHandler::CPLHandler(ReplicatedLog* apoLog)
{
    mpoLog = apoLog;
}

MINA_NAMESPACE_END
