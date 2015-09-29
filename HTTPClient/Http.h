#ifndef _CMINA_HTTP_INCLUDE_
#define _CMINA_HTTP_INCLUDE_

#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include "Define.h"
#include "Memblock.h"


#define CMINA_ERROR_METHOD -1
#define CMINA_ERROR_URI -2
#define CMINA_ERROR_VERSION -3;

#define CMINA_HTTP_COPY     1
#define CMINA_HTTP_DELETE   2
#define CMINA_HTTP_GET      3
#define CMINA_HTTP_HEAD     4
#define CMINA_HTTP_LOCK     5
#define CMINA_HTTP_MKCOL    6
#define CMINA_HTTP_MOVE     7
#define CMINA_HTTP_OPTIONS  8
#define CMINA_HTTP_PATCH    9
#define CMINA_HTTP_POST     10
#define CMINA_HTTP_PROFIND  11
#define CMINA_HTTP_PROPPACHE 12
#define CMINA_HTTP_PUT      13
#define CMINA_HTTP_TRACE    14
#define CMINA_HTTP_UNLOCK   15


class CHttp
{
public:
    uint32_t muVersion;
    uint32_t muMethod;
    char *mpcURI;
    mina::CMemblock *mpoBlock;
    uint32_t muStatus;
    char* mpcReson;
public:
    void SetBlock(mina::CMemblock *apoBlock);
    mina::CMemblock *GetBlock(void);
    uint32_t GetVersion(void);
    uint32_t GetMethod(void);
    uint32_t GetStatus(void);
    void SetVersion(uint32_t auStatus);
    void SetMethod(uint32_t auStatus);
    void SetStatus(uint32_t auStatus);
    void SetURI(char* apcURI);
    char* GetURI(void);
    void SetReson(char* apcURI);
    char* GetReson(void);

    int DecodeStartLine(void);
    int Encode(void);

};


#endif
