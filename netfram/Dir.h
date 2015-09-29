#ifndef _DIR_INCLUDE_
#define _DIR_INCLUDE_
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <assert.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/un.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string>
#include "Define.h"

MINA_NAMESPACE_START

class CDir
{
public:
    static int Make(char const* apcDirPath, int aiMode = 0700);

    static int Remove(char const* apcDirPath);
private:

};
MINA_NAMESPACE_END
#endif
