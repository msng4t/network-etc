
#ifndef _CMINA_HANDLER_H_INCLUDE_
#define _CMINA_HANDLER_H_INCLUDE_

#include "IoHandler.h"
#include "Define.h"
#include "Synch.h"
#include "ReplicatedLog.h"

MINA_NAMESPACE_START

class CPLHandler: public CIoHandler
{
public:
    virtual ~CPLHandler(void) {};
    CPLHandler(ReplicatedLog* apoLog);

    virtual void SessionException(SessionType aoSession, CThrowable const& aoCause );
    virtual void SessionOpen(SessionType aoSession);
    virtual void SessionClose(SessionType aoSession);
    virtual void SessionIdle(SessionType aoSession, uint32_t status);
    virtual void MessageRecived(SessionType aoSession, CAny& aoMessage);
    virtual void MessageFinished(SessionType aoSession, CAny& aoMessage);
    virtual void FilterClose(SessionType aoSession);
    virtual void FilterWrite(SessionType aoSession, CAny& aoMessage);
private:
    ReplicatedLog* mpoLog;
};

MINA_NAMESPACE_END
#endif
