#ifndef _SOCK_PAIR_INCLUDE_
#define _SOCK_PAIR_INCLUDE_
#include <sys/socket.h>
#include <sys/types.h>
#include "Define.h"

MINA_NAMESPACE_START

class CSockPair
{
public:
    int Open( void );
    int GetFirst( void );
    int GetSecond( void );

private:
    int maiFds[2];
};

MINA_NAMESPACE_END
#endif
