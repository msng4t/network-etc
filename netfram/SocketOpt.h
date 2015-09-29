#ifndef _CMINA_SOCKET_H_INCLUDE_
#define _CMINA_SOCKET_H_INCLUDE_
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>


#define MINA_NONBLOCKING(s) fcntl(s, F_SETFL, fcntl(s,F_GETFL) | O_NONBLOCK)
#define MINA_BLOCKING(s) fcntl(s, F_SETFL, fcntl(s,F_GETFL) & ~O_NONBLOCK)

int MinaTcpPush(int aiSock);
int MinaTcpNopush(int aiSock);





#endif
