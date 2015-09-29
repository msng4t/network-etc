#ifndef _TIMEOUT_EVENT_INCLUDE_
#define _TIMEOUT_EVENT_INCLUDE_
#include "Define.h"

MINA_NAMESPACE_START

class CTimeoutHandler
{
public:
    virtual ~CTimeoutHandler(void) = 0;
    virtual int HandleTimeout() = 0;
};
MINA_NAMESPACE_END
#endif
