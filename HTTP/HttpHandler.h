
#ifndef _CMINA_HANDLER_H_INCLUDE_
#define _CMINA_HANDLER_H_INCLUDE_

#include "IoHandler.h"
#include "Define.h"
#include "Synch.h"

MINA_NAMESPACE_START

class CHttpHandler: public CIoHandler
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

class CIDGenerator
{
public:
    CIDGenerator(void);
    uint64_t GetID(void);
private:
    uint64_t muCurId;
    CLock moLock;
};

uint64_t GetNextId(void);

MINA_NAMESPACE_END
#endif
