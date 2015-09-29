#include <assert.h>
#include "Memblock.h"


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
    if (this->miFlag | MEM_KEEP_TERM)
    {
        this->mpcEnd = lpcAddr + auSize - 1;
        *(this->mpcEnd + 1) = '\0';
    }
    else
    {
        this->mpcEnd = lpcAddr + auSize;
    }
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
    if (this->miFlag | MEM_KEEP_TERM)
    {
        this->mpcEnd = this->mpcBase + auSize - 1;
        *(this->mpcEnd + 1) = '\0';
    }
    else
    {
        this->mpcEnd = this->mpcBase + auSize;
    }

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

ssize_t
CMemblock::Write(char const* apcBuffer, size_t auSize)
{
    size_t luRemain;

    if (this->miFlag | MEM_KEEP_TERM)
    {
        luRemain = this->muCapacity - this->muSize - auSize - 1;
    }
    else
    {
        luRemain = this->muCapacity - this->muSize - auSize;
    }

    if (luRemain < 0)
    {
        errno = ENOSPC;
        return -1;
    }

    if (size_t(this->mpcEnd - this->mpcWritePt) < auSize)
    {
        luRemain = this->mpcWritePt - this->mpcReadPt;
        memmove(this->mpcBase, this->mpcReadPt, luRemain);
        this->mpcReadPt = this->mpcBase;
        this->mpcWritePt = this->mpcBase + luRemain;
    }

    memcpy(this->mpcWritePt, apcBuffer, auSize);
    this->mpcWritePt = this->mpcWritePt + auSize;
    this->muSize += auSize;

    return auSize;
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
    return this->mpcWritePt - this->mpcReadPt;
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

char* CMemblock::GetWrite(void)
{
    return this->mpcWritePt;
}

size_t CMemblock::GetDerectSpace(void)
{
    return this->mpcEnd - this->mpcWritePt;
}

char* CMemblock::GetRead(void)
{
    return this->mpcReadPt;
}

size_t CMemblock::GetCapacity(void)
{
    return this->muCapacity;
}

size_t
CMemblock::GetSize(void)
{
    return this->muSize;
}
size_t CMemblock::GetSpace(void)
{
    if (this->miFlag | MEM_KEEP_TERM)
    {
        return this->muCapacity - this->muSize - 1;
    }
    else
    {
        return this->muCapacity - this->muSize;
    }
}

