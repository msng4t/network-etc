#include "SockPair.h"

MINA_NAMESPACE_START

int CSockPair::Open( void )
{
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, maiFds) != -1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int CSockPair::GetFirst( void )
{
    return maiFds[0];
}
int CSockPair::GetSecond( void )
{
    return maiFds[1];
}


MINA_NAMESPACE_END
