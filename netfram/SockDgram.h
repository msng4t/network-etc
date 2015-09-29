#ifndef _SOCK_UDP_INCLUDE_
#define _SOCK_UDP_INCLUDE_
#include <sys/socket.h>
#include <sys/types.h>
#include "SockAddr.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "Define.h"

MINA_NAMESPACE_START

class CSockDgram
{
public:
    CSockDgram();
    int Open( CSockAddr& aoAddr );
    int Open( void );
    int GetFd( void );
    int Join( CSockAddr& aoGroup, char const* apcIfName = NULL, unsigned auIfIndex = 0);
    int Leave(CSockAddr& aoGroup);
    CSockAddr& GetLocal( void );
private:
    int miFd;
    CSockAddr moLocal;
};

MINA_NAMESPACE_END
#endif
