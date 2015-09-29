#include "SocketOpt.h"



int 
MinaTcpPush(int aiSock)
{
#if 0
    int cork;

    cork = 0;

    return setsockopt(aiSock, IPPROTO_TCP, TCP_NOPUSH, (const void*)&cork, sizeof(int));
#else
    return 0;
#endif
}

int 
MinaTcpNopush(int aiSock)
{
#if 0
    int cork;

    cork = 1;

    return setsockopt(aiSock, IPPROTO_TCP, TCP_CORK, (const void*)&cork, sizeof(int));
#else
    return 0;
#endif
}

