#include "HttpHandler.h"
#include "Memblock.h"
#include "IoEvent.h"
#include "Log.h"
#include "Singleton.h"
#include "HttpRequest.h"
#include "FilterChain.h"
#include "IoConnector.h"
#include "SessionManager.h"
#include "MessageTable.h"
#include "HttpContentHandler.h"

MINA_NAMESPACE_START

static void MinaReadIdle(CIoEvent *ev);
static void MinaWriteIdle(CIoEvent *ev);

void CHttpHandler::SessionException(SessionType aoSession, CThrowable const& aoCause )
{

    LOG_DEBUG("test session_Exception");
}
void CHttpHandler::SessionOpen(SessionType aoSession)
{
    CSockAddr loAddr;
    CSockAddr loLocal;
    loAddr.GetRemote(aoSession->GetFd());
    loLocal.GetLocal(aoSession->GetFd());
    LOG_DEBUG("test (%s:%d to %s:%d", loAddr.GetIP(), loAddr.GetPort(), loLocal.GetIP(), loLocal.GetPort());
}

void CHttpHandler::SessionClose(SessionType aoSession)
{
//    MessageTableType* lpoMessageTable;

//    lpoMessageTable = GetCycle()->GetMessageTable();
//
//    CMessageNode loMessageNode;
//    if (GetCycle()->GetMessageTable()->Get(aoSession->GetNumber(), loMessageNode))
//    {
//        GetCycle()->GetMessageTable()->Delete(loMessageNode.GetOutSessionID());
//        GetCycle()->GetMessageTable()->Delete(loMessageNode.GetInSessionID());
//        LOG_DEBUG("MTable delete %lld and %lld", loMessageNode.GetInSessionID(),
//                loMessageNode.GetOutSessionID());
//    }
//
    LOG_DEBUG("test session_close_handler, R:%x", aoSession->GetRead());
}

void CHttpHandler::SessionIdle(SessionType aoSession, uint32_t status)
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
void CHttpHandler::MessageRecived(SessionType aoSession, CAny& aoMessage)
{
    int liRet = -1;
    HttpRequest request;
    CMemblock* reqblock = *aoMessage.CastTo<CMemblock*>();
    CMemblock* response;
    ContentHandler loHandler;
    response = aoSession->GetWrite()->GetBlock();
    request.Init();

    liRet = request.Parse(reqblock->GetRead(), reqblock->GetSize());
    if (liRet > 0)
    {

        LOG_DEBUG("===========================");
        LOG_DEBUG("%-10s:\t%-20s", "method", request.moRequestLine.method);
        LOG_DEBUG("%-10s:\t%-20s", "URI", request.moRequestLine.uri);
        LOG_DEBUG("%-10s:\t%-20s", "wersion", request.moRequestLine.version);
        LOG_DEBUG("Headers:");
        char *lpcData = request.moHeader.mpcData;
        for (int liN = 0; liN < request.moHeader.miHeaderSize; liN++)
        {
            LOG_DEBUG("%-10s:\t%-20s",
                    lpcData + request.moHeader.mpoHeaders[liN].keyStart,
                    lpcData + request.moHeader.mpoHeaders[liN].keyStart);
        }
        LOG_DEBUG("===========================");
    }
    else{
        reqblock = *aoMessage.CastTo<CMemblock*>();
        reqblock->Reset();
    }
    loHandler.HandleRequest(response, request);
    CAny loResponse(response);
    aoSession->Write(loResponse);

}

void CHttpHandler::MessageFinished(SessionType aoSession, CAny& aoMessage)
{
    LOG_DEBUG("test filter_finished_handler");

}
void CHttpHandler::FilterClose(SessionType aoSession)
{
    LOG_DEBUG("test filter_close_handler");

}
void CHttpHandler::FilterWrite(SessionType aoSession, CAny& aoMessage)
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


CIDGenerator::CIDGenerator(void)
    : muCurId (0)
{

}

uint64_t CIDGenerator::GetID(void)
{
    TSmartLock<CLock> loGuard(this->moLock);
    return ++this->muCurId;
}

uint64_t GetNextId(void)
{
    return Instance<CIDGenerator>()->GetID();
}

MINA_NAMESPACE_END
