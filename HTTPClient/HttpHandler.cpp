#include "HttpHandler.h"
#include "Memblock.h"
#include "IoEvent.h"
#include "Log.h"
#include "Singleton.h"
#include "HttpResponse.h"
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
    LOG_DEBUG("MessageRecived===========================");
    CMemblock* reqblock = *aoMessage.CastTo<CMemblock*>();
    CMemblock* lpoResBlock;
    ContentHandler loHandler;
    lpoResBlock = aoSession->GetWrite()->GetBlock();
    HttpResponse* lpoResponse;

    if (aoSession->GetData() == NULL)
    {
      lpoResponse = new HttpResponse;
      lpoResponse->Init();
    }
    else
    {
      lpoResponse = (HttpResponse*) aoSession->GetData();
    }

    LOG_DEBUG("%s", reqblock->GetRead());
    liRet = lpoResponse->Parse(reqblock->GetRead(), reqblock->GetSize());
    if (liRet > 0)
    {
      if (lpoResponse->moHeader.ChunkFinished())
      {
        LOG_DEBUG("===========================");
        LOG_DEBUG("%-10s:\t%-20s", "reason", lpoResponse->moStatusLine.reason);
        LOG_DEBUG("%-10s:\t%-20s", "code", lpoResponse->moStatusLine.code);
        LOG_DEBUG("%-10s:\t%-20s", "version", lpoResponse->moStatusLine.version);
        LOG_DEBUG("Headers:");
        char *lpcData = lpoResponse->moHeader.mpcData;
        for (int liN = 0; liN < lpoResponse->moHeader.miHeaderSize; liN++)
        {
          LOG_DEBUG("%-10s:\t%-20s",
              lpcData + lpoResponse->moHeader.mpoHeaders[liN].keyStart,
              lpcData + lpoResponse->moHeader.mpoHeaders[liN].valueStart);
        }
        for (int liN = 0; liN < lpoResponse->moHeader.miChunkSize; liN++)
        {
          LOG_DEBUG("chunk:%-100s", lpcData + lpoResponse->moHeader.mpoChunks[liN].Start);
        }
        LOG_DEBUG("===========================");
        loHandler.HandleResponse(lpoResBlock, *lpoResponse);
        reqblock->Reset();
      }
      else
      {
        reqblock->SetRead(liRet);
        aoSession->SetData(lpoResponse);
      }
    }
    else{
        reqblock = *aoMessage.CastTo<CMemblock*>();
        reqblock->Reset();
    }

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
