#ifndef _CMINA_MEMBLOCK_H_INCLUDE_
#define _CMINA_MEMBLOCK_H_INCLUDE_
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#define MEM_DONT_DELETE     1
#define MEM_DO_DELETE       2
#define MEM_BLOCK_RELEASED  4
#define MEM_KEEP_TERM       8

class CMemblock
{
public:

    CMemblock(void);
    CMemblock(size_t auSize);
    ~CMemblock(void);
    int Init(
            size_t auSize,
            char *mpcAddr = NULL,
            char const* mpcData = NULL,
            size_t miLen = 0,
            int miFlag = MEM_DO_DELETE);

    void Release();

    CMemblock* Clone();

    ssize_t Write(char const* mpcBuffer, size_t auSize);

    ssize_t Read(char* mpcBuffer, size_t auSize);

    void Reset(void);
    void Shrink(void);
    void Resize(size_t auSize);

    int SetWrite(size_t auSize);
    int SetWrite(char* apcWrite);
    char* GetWrite(void);
    char* GetRead(void);
    int SetRead(size_t auSize);
    size_t GetCapacity(void);
    size_t GetSize(void);
    size_t GetSpace(void);
    size_t GetDerectSpace(void);
private:
    char *mpcBase;
    char *mpcEnd;
    char *mpcReadPt;
    char *mpcWritePt;
    size_t muCapacity;
    size_t muSize;
    int miFlag;

};

#endif
