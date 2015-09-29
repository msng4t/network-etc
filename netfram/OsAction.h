#ifndef _CMINA_OS_H_INCLUDE_
#define _CMINA_OS_H_INCLUDE_
#include "Define.h"
#include "Session.h"


MINA_NAMESPACE_START
class COsAction
{
public:
    static ssize_t RecvChain(CSession *apoSession, char *apcBuffer);
    static ssize_t SendChain(CSession *apoSession, char *apcBuffer);
    static ssize_t Send(CSession *apoSession, char *apcBuffer, size_t auSize);
    static ssize_t SendUDP(CSession *apoSession, char *apcBuffer, size_t auSize);
    static ssize_t RecvUDP(CSession *apoSession,
            char *apcBuffer,
            size_t auSize);
    static ssize_t Recv(CSession *apoSession, char *apcBuffer, size_t auSize);

    static int CheckStatus(CSession *apoSession);
    uint32_t muFlags;

};

MINA_NAMESPACE_END
#endif
