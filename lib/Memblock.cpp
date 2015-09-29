#include <assert.h>
#include "Memblock.h"

MINA_NAMESPACE_START

CMemblock::CMemblock(void)
    : mpcBase (NULL)
    , mpcEnd (NULL)
    , mpcReadPt (NULL)
    , mpcWritePt (NULL)
    , muCapacity (0)
    , muSize (0)
    , miFlag (0)
{
}

CMemblock::CMemblock(size_t auSize)
{
    this->Init(auSize);
}

CMemblock::~CMemblock(void)
{
    if (this->miFlag ^ MEM_DONT_DELETE)
    {
        this->Release();
    }
}

void CMemblock::Resize(size_t auSize)
{
    char* lpcAddr = NULL;

    if (this->muCapacity >= auSize)
    {
        return;
    }

    lpcAddr = new char[auSize];
    assert(lpcAddr != NULL);
    memmove(lpcAddr, this->mpcReadPt, this->muSize);

    if (this->miFlag == MEM_DO_DELETE)
    {
        if (this->mpcBase != NULL)
            delete this->mpcBase;
    }

    this->mpcBase = lpcAddr;
    this->mpcEnd = lpcAddr + auSize;
    this->mpcReadPt = lpcAddr;
    this->mpcWritePt = lpcAddr + this->muSize;
    this->muCapacity = auSize;


}

int
CMemblock::Init(
        size_t auSize,
        char *apcAddr,
        char const* apcData,
        size_t aiLen,
        int aiFlag)
{
    if (auSize <= 0 || aiLen > auSize) return -1;

    this->mpcBase = apcAddr;

    if (this->mpcBase == NULL)
    {
        this->mpcBase = new char[auSize];
    }
    assert(this->mpcBase != NULL);

    this->muCapacity = auSize;
    this->mpcEnd = this->mpcBase + auSize;
    this->mpcReadPt = this->mpcBase;
    this->mpcWritePt = this->mpcBase;
    if (apcData == NULL)
    {
        this->muSize = 0;
    }
    else if (this->mpcBase == apcData)
    {
        this->muSize = aiLen;
        this->mpcWritePt += aiLen;
    }
    else
    {
        this->muSize = aiLen;
        memcpy(this->mpcWritePt, (void *)apcData, aiLen);
        this->mpcWritePt += aiLen;
    }
    this->miFlag = aiFlag;

    return 0;
}

void
CMemblock::Release(void)
{
    if (this->mpcBase != NULL)
    {
        delete[] this->mpcBase;
    }

    this->mpcBase = NULL;
    this->mpcEnd = NULL;
    this->muCapacity = 0;

    this->mpcReadPt = NULL;
    this->mpcWritePt = NULL;
    this->muSize = 0;
    this->miFlag = MEM_BLOCK_RELEASED;
}

CMemblock*
CMemblock::Clone()
{
    CMemblock* loCloned;
    int ret;

    if (this == NULL) return NULL;

    loCloned = new CMemblock();
    if (loCloned == NULL) return NULL;

    ret = loCloned->Init(
            this->mpcEnd - this->mpcBase,
            NULL,
            this->mpcReadPt,
            this->mpcWritePt - this->mpcReadPt,
            this->miFlag);
    if (ret != 0)
    {
        free(loCloned);
        loCloned = NULL;
    }

    return loCloned;
}

void CMemblock::Expand(size_t auSize)
{
    ssize_t liRemain;

    liRemain = this->muCapacity - this->muSize - auSize;
    if (liRemain < 0)
    {
        size_t luSize = this->muCapacity;
        while(true)
        {
            luSize <<= 2;
            if ((luSize - this->muSize - auSize) > 0)
                break;
        }
        this->Resize(luSize);
    }
}
ssize_t
CMemblock::Write(char const* apcBuffer, size_t auSize)
{
    ssize_t liRemain;

    this->Expand(auSize);
    if (size_t(this->mpcEnd - this->mpcWritePt) < auSize)
    {
        liRemain = this->mpcWritePt - this->mpcReadPt;
        memmove(this->mpcBase, this->mpcReadPt, liRemain);
        this->mpcReadPt = this->mpcBase;
        this->mpcWritePt = this->mpcBase + liRemain;
    }

    memcpy(this->mpcWritePt, apcBuffer, auSize);
    this->mpcWritePt = this->mpcWritePt + auSize;
    this->muSize += auSize;

    return auSize;
}

ssize_t CMemblock::Write(uint8_t  au8Num)
{
    this->Expand(1);
    *this->mpcWritePt = (unsigned char)au8Num;
    this->SetWrite(1);
    return 1;
}

ssize_t CMemblock::Write(uint16_t au16Num)
{
    this->Expand(2);
    mpcWritePt[1] = (unsigned char)au16Num;
    au16Num = au16Num >> 8;
    mpcWritePt[0] = (unsigned char)au16Num;
    this->SetWrite(2);
    return 2;
}

ssize_t CMemblock::Write(uint32_t au32Num)
{
    this->Expand(4);
    mpcWritePt[3] = (unsigned char)au32Num;
    au32Num = au32Num >> 8;
    mpcWritePt[2] = (unsigned char)au32Num;
    this->SetWrite(2);
    mpcWritePt[1] = (unsigned char)au32Num;
    au32Num = au32Num >> 8;
    mpcWritePt[0] = (unsigned char)au32Num;
    this->SetWrite(4);
    return 4;

}
ssize_t CMemblock::Write(uint64_t au64Num)
{
    this->Expand(8);
    mpcWritePt[7] = (unsigned char)au64Num;
    au64Num = au64Num >> 8;
    mpcWritePt[6] = (unsigned char)au64Num;
    this->SetWrite(2);
    mpcWritePt[5] = (unsigned char)au64Num;
    au64Num = au64Num >> 8;
    mpcWritePt[4] = (unsigned char)au64Num;
    mpcWritePt[3] = (unsigned char)au64Num;
    au64Num = au64Num >> 8;
    mpcWritePt[2] = (unsigned char)au64Num;
    this->SetWrite(2);
    mpcWritePt[1] = (unsigned char)au64Num;
    au64Num = au64Num >> 8;
    mpcWritePt[0] = (unsigned char)au64Num;
    this->SetWrite(8);
    return 8;

}

ssize_t CMemblock::Write(std::vector<uint8_t>  &aoVec)
{
    uint32_t luSize = aoVec.size();
    this->Write(luSize);
    for(uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        this->Write(aoVec[luCount]);
    }
    return 4 + luSize;
}

ssize_t CMemblock::Write(std::vector<uint16_t> &aoVec)
{
    uint32_t luSize = aoVec.size();
    this->Write(luSize);
    for(uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        this->Write(aoVec[luCount]);
    }
    return 4 + 2 * luSize;
}

ssize_t CMemblock::Write(std::vector<uint32_t> &aoVec)
{
    uint32_t luSize = aoVec.size();
    this->Write(luSize);
    for(uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        this->Write(aoVec[luCount]);
    }
    return 4 *( 1 + luSize);
}

ssize_t CMemblock::Write(std::vector<uint64_t> &aoVec)
{
    uint32_t luSize = aoVec.size();
    this->Write(luSize);
    for(uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        this->Write(aoVec[luCount]);
    }
    return 4 *( 1 + 2 * luSize);
}

ssize_t
CMemblock::Read(char* apcBuffer, size_t auSize)
{
    size_t remain;
    if (this->muSize == 0)
    {
        errno = ENODATA;
        return -1;
    }
    else if (this->muSize < auSize)
    {
        remain = this->muSize;
        memcpy(apcBuffer, this->mpcReadPt, this->muSize);
        this->mpcReadPt =  this->mpcWritePt;
        this->muSize = 0;
        return remain;
    }
    else
    {
        memcpy(apcBuffer, this->mpcReadPt, auSize);
        this->mpcReadPt += auSize;
        this->muSize -= auSize;
        return auSize;
    }
}

ssize_t CMemblock::Read(uint8_t  &au8Num)
{
    if (this->muSize < 1)
    {
        return -1;
    }

    au8Num = static_cast<uint8_t>(*this->mpcReadPt);
    this->SetRead(1);
    return 0;
}

ssize_t CMemblock::Read(uint16_t &au16Num)
{
    if (this->muSize < 2)
    {
        return -1;
    }

    au16Num = mpcReadPt[0];
    au16Num <<= 8;
    au16Num |= mpcReadPt[1];
    this->SetWrite(2);
    return 0;
}

ssize_t CMemblock::Read(uint32_t &au32Num)
{
    if (this->muSize < 2)
    {
        return -1;
    }
    au32Num = mpcReadPt[0];
    au32Num <<= 8;
    au32Num |= mpcReadPt[1];
    au32Num <<= 8;
    au32Num |= mpcReadPt[2];
    au32Num <<= 8;
    au32Num |= mpcReadPt[3];
    au32Num <<= 8;

    return 0;
}
ssize_t CMemblock::Read(uint64_t &au64Num)
{
    if (this->muSize < 2)
    {
        return -1;
    }

    au64Num = mpcReadPt[0];
    au64Num <<= 8;
    au64Num |= mpcReadPt[1];
    au64Num <<= 8;
    au64Num |= mpcReadPt[2];
    au64Num <<= 8;
    au64Num |= mpcReadPt[3];
    au64Num <<= 8;
    au64Num = mpcReadPt[0];
    au64Num <<= 8;
    au64Num |= mpcReadPt[1];
    au64Num <<= 8;
    au64Num |= mpcReadPt[2];
    au64Num <<= 8;
    au64Num |= mpcReadPt[3];
    au64Num <<= 8;
    this->SetRead(8);
    return 0;
}

ssize_t CMemblock::Read(std::vector<uint8_t>  &aoVec)
{
    uint32_t luSize;
    uint8_t  luResult;
    if (this->Read(luSize) != 0)
    {
        return -1;
    }

    for (uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        if (Read(luResult) != 0)
        {
            aoVec.clear();
            return -1;
        }
        aoVec.push_back(luResult);
    }
    return 4 + luSize;
}

ssize_t CMemblock::Read(std::vector<uint16_t> &aoVec)
{
    uint32_t luSize;
    uint16_t  luResult;
    if (this->Read(luSize) != 0)
    {
        return -1;
    }

    for (uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        if (Read(luResult) != 0)
        {
            aoVec.clear();
            return -1;
        }
        aoVec.push_back(luResult);
    }
    return 4 + luSize * 2;
}

ssize_t CMemblock::Read(std::vector<uint32_t> &aoVec)
{
    uint32_t luSize;
    uint32_t  luResult;
    if (this->Read(luSize) != 0)
    {
        return -1;
    }

    for (uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        if (Read(luResult) != 0)
        {
            aoVec.clear();
            return -1;
        }
        aoVec.push_back(luResult);
    }
    return 4 + luSize * 4;

}

ssize_t CMemblock::Read(std::vector<uint64_t> &aoVec)
{
    uint32_t luSize;
    uint64_t  luResult;

    if (this->Read(luSize) != 0)
    {
        return -1;
    }

    for (uint32_t luCount = 0; luCount < luSize; luCount++)
    {
        if (Read(luResult) != 0)
        {
            aoVec.clear();
            return -1;
        }
        aoVec.push_back(luResult);
    }
    return 4 + luSize * 4;
}

void CMemblock::Shrink(void)
{
    memmove(this->mpcBase, this->mpcReadPt, this->muSize);
    this->mpcReadPt = this->mpcBase;
    this->mpcWritePt = this->mpcBase + this->muSize;
}

void
CMemblock::Reset()
{
    this->mpcReadPt = this->mpcBase;
    this->mpcWritePt = this->mpcBase;
    this->muSize = 0;
}


int
CMemblock::SetWrite(size_t auSize)
{

    if (size_t(this->mpcEnd - this->mpcWritePt) < auSize)
    {
        errno = ENOSPC;
        return -1;
    }


    this->mpcWritePt = this->mpcWritePt + auSize;
    this->muSize += auSize;

    return auSize;
}

int
CMemblock::SetWrite(char* apcWrite)
{
    if (this->mpcReadPt > apcWrite || this->mpcEnd < apcWrite)
    {
        return -1;
    }
    this->mpcWritePt = apcWrite;
    this->muSize = this->mpcWritePt - this->mpcReadPt;
    return this->muSize;
}

int
CMemblock::SetRead(size_t auSize)
{
    if ((this->muSize == 0)
        || (this->muSize < auSize))
    {
        errno = ENODATA;
        return -1;
    }
    else
    {
        this->mpcReadPt += auSize;
        this->muSize -= auSize;
        return auSize;
    }
}

int
CMemblock::SetRead(char* apcReadPt)
{
    if (this->mpcWritePt < apcReadPt || this->mpcBase < apcReadPt)
    {
        return -1;
    }
    char* lpcRead = this->mpcReadPt;
    this->mpcReadPt = apcReadPt;
    this->muSize = this->mpcWritePt - this->mpcReadPt;

    return this->mpcReadPt - lpcRead;
}

char* CMemblock::GetWrite(void) const
{
    return this->mpcWritePt;
}

size_t CMemblock::GetDirectSpace(void) const
{
    return this->mpcEnd - this->mpcWritePt;
}

char* CMemblock::GetRead(void) const
{
    return this->mpcReadPt;
}

size_t CMemblock::GetCapacity(void) const
{
    return this->muCapacity;
}

size_t
CMemblock::GetSize(void) const
{
    return this->muSize;
}
size_t CMemblock::GetSpace(void) const
{
    return this->muCapacity - this->muSize;
}

MINA_NAMESPACE_END
