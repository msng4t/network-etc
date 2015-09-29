#ifndef _EXCEPTION_INCLUDE_
#define _EXCEPTION_INCLUDE_
#include <string>
#include "Define.h"

MINA_NAMESPACE_START

/**
 * Exception state define
 */
enum EExceptionState
{
    //Sock accept exception
    E_SOCK_BIND = -100,

    //Sock accept exception
    E_SOCK_ACCEPT = -101,

    //Sock connect exception
    E_SOCK_CONNECT = -102,

    //SOCK send exception
    E_SOCK_SEND = -103,

    //SOCK recv exception
    E_SOCK_RECV = -104,

    //Poll add handle exception
    E_POLL_ADD = -105,

    //Poll del handle exception
    E_POLL_DEL = -106,

    //Poll select handle exception
    E_POLL_SELECT = -107,

    //Poll invalid event
    E_POLL_INVALID_EVENT = -108,

    //Invalid message
    E_INVALID_MESSAGE = -109,

    //Window full
    E_WINDOW_FULL = -110,

    //Timeout
    E_TIMEOUT = -111,
};

class CThrowable
{
public:
    CThrowable(int aiState, int aiErrno, char const* apcMessage);

    /**
     * Return anf exception state number
     */
    int GetState(void) const;

    int GetErrno(void) const;

    char const* GetMessage(void) const;

protected:
    int miState;

    int miErrno;

    std::string moMessage;
};

MINA_NAMESPACE_END

#endif//ANF_EXCEPTION_H
