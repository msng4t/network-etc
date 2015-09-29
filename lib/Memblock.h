#ifndef _CMINA_MEMBLOCK_H_INCLUDE_
#define _CMINA_MEMBLOCK_H_INCLUDE_
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <vector>
#include "Define.h"


#define MEM_DONT_DELETE     1
#define MEM_DO_DELETE       2
#define MEM_BLOCK_RELEASED  4
#define MEM_KEEP_TERM       8

MINA_NAMESPACE_START
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
    ssize_t Write(uint8_t  au8Num);
    ssize_t Write(uint16_t au16Num);
    ssize_t Write(uint32_t au32Num);
    ssize_t Write(uint64_t au64Num);
    ssize_t Write(std::vector<uint8_t>  &aoVec);
    ssize_t Write(std::vector<uint16_t> &aoVec);
    ssize_t Write(std::vector<uint32_t> &aoVec);
    ssize_t Write(std::vector<uint64_t> &aoVec);

    ssize_t Read(char* mpcBuffer, size_t auSize);
    ssize_t Read(uint8_t  &au8Num);
    ssize_t Read(uint16_t &au16Num);
    ssize_t Read(uint32_t &au32Num);
    ssize_t Read(uint64_t &au64Num);
    ssize_t Read(std::vector<uint8_t>  &aoVec);
    ssize_t Read(std::vector<uint16_t> &aoVec);
    ssize_t Read(std::vector<uint32_t> &aoVec);
    ssize_t Read(std::vector<uint64_t> &aoVec);

    void Reset(void);
    void Shrink(void);
    void Resize(size_t auSize);
    void Expand(size_t auSize);

    int SetWrite(size_t auSize);
    int SetWrite(char* apcWrite);
    char* GetWrite(void) const;
    char* GetRead(void) const;
    int SetRead(size_t auSize);
    int SetRead(char* apcRead);
    size_t GetCapacity(void) const;
    size_t GetSize(void) const;
    size_t GetSpace(void) const;
    size_t GetDirectSpace(void) const;
    std::string ToString() const {return std::string(mpcReadPt, muSize);}

    int Compare( const CMemblock& aoOther)
    {
        return ((this->muSize >= aoOther.GetSize()) &&
                    (memcmp(this->mpcReadPt, aoOther.GetRead(), aoOther.muSize)));
    }
private:
    char *mpcBase;
    char *mpcEnd;
    char *mpcReadPt;
    char *mpcWritePt;
    size_t muCapacity;
    size_t muSize;
    int miFlag;

};

MINA_NAMESPACE_END
#endif
