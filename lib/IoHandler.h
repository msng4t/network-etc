#ifndef _IO_HANDLER_INCLUDE_
#define _IO_HANDLER_INCLUDE_
#include <stdint.h>
#include <inttypes.h>
#include "Define.h"
#include "Exception.h"
#include "Any.h"


MINA_NAMESPACE_START
class CSession;
typedef CSession* SessionType;

class CIoHandler
{
public:
    virtual ~CIoHandler(void) {};

    virtual void SessionException(SessionType aoSession, CThrowable const& aoCause ) = 0;
    virtual void SessionOpen(SessionType aoSession) = 0;
    virtual void SessionClose(SessionType aoSession) = 0;
    virtual void SessionIdle(SessionType aoSession, uint32_t status) = 0;
    virtual void MessageRecived(SessionType aoSession, CAny& aoMessage) = 0;
    virtual void MessageFinished(SessionType aoSession, CAny& aoMessage) = 0;
    virtual void FilterClose(SessionType aoSession) = 0;
    virtual void FilterWrite(SessionType aoSession, CAny& aoMessage) = 0;
};


MINA_NAMESPACE_END
#endif
