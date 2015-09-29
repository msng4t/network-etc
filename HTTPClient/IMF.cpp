#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "IMF.h"

#define CR					13
#define LF					10
#define CS_CR				"\015"
#define CS_LF				"\012"
#define CS_CRLF				CS_CR CS_LF

#ifdef WIN32
#define strncasecmp _strnicmp
#endif

static char* SkipWhitespace(char* p, int len)
{
	if (!p)
		return NULL;

	if (len <= 0)
		return NULL;

	while (p && *p && *p <= ' ')
	{
		if (--len == 0)
			return NULL;

		p++;
	}

	if (!*p)
		return NULL;

	return p;
}

static char* SeekWhitespace(char* p, int len)
{
	if (!p)
		return NULL;

	if (len <= 0)
		return NULL;

	while (p && *p && *p != ' ')
	{
		if (--len == 0)
			return NULL;

		p++;
	}

	if (!*p)
		return NULL;

	return p;
}

static char* SkipCrlf(char* p, int len)
{
	if (!p)
		return NULL;

	if (len < 2)
		return NULL;

	while (*p && (*p == CS_CR[0] || *p == CS_LF[0]))
	{
		if (--len == 0)
			return NULL;

		p++;
	}

	if (!*p)
		return NULL;

	return p;
}

static char* SeekCrlf(char* p, int len)
{
	if (!p)
		return NULL;
	
	if (len < 2)
		return NULL;
	
	while (*p && (p[0] != CS_CR[0] && p[1] != CS_LF[0]))
	{
		if (--len == 0)
			return NULL;

		p++;
	}
	
	if (!*p)
		return NULL;
	
	return p;	
}

static char* SeekChar(char* p, int len, char c)
{
	if (!p)
		return NULL;
	
	if (len < 1)
		return NULL;
	
	while (*p && *p != c)
	{
		if (--len == 0)
			return NULL;
		
		p++;
	}
	
	if (!*p)
		return NULL;
	
	return p;
}

static int LineParse(char* buf, int len, int offs, const char** values[3])
{
	char* p;

#define remlen (len - (p - buf))
	// p is set so that in remlen it won't be uninitialized
	p = buf;	
	p = SkipWhitespace(buf + offs, remlen);
	
	*values[0] = p;
	p = SeekWhitespace(p, remlen);
	if (!p) return -1;
	
	*p++ = '\0';
	p = SkipWhitespace(p, remlen);
	if (!p) return -1;
	
	*values[1] = p;
	p = SeekWhitespace(p, remlen);
	if (!p) return -1;
	
	*p++ = '\0';
	p = SkipWhitespace(p, remlen);
	if (!p) return -1;
	
	*values[2] = p;
	p = SeekCrlf(p, remlen);
	if (!p) return -1;
	
	*p = '\0';
	p += 2;
	
	return (int) (p - buf);	

#undef remlen
}

int CIMFHeader::RequestLine::Parse(char* buf, int len, int offs)
{
	const char** values[3];
	method = NULL;
	methodLen = -1;
	uri = NULL;
	uriLen = -1;
	version = NULL;
	versionLen = -1;
	
	values[0] = &method;
	values[1] = &uri;
	values[2] = &version;
	
	return LineParse(buf, len, offs, values);
}

int CIMFHeader::StatusLine::Parse(char* buf, int len, int offs)
{
	const char** values[3];
	version = NULL;
	versionLen = -1;
	code = NULL;
	codeLen = -1;
	reason = NULL;
	reasonLen = -1;
	
	values[0] = &version;
	values[1] = &code;
	values[2] = &reason;
	
	return LineParse(buf, len, offs, values);
}

CIMFHeader::CIMFHeader()
{
	Init();
}

CIMFHeader::~CIMFHeader()
{
	Free();
}

void CIMFHeader::Init()
{
	miHeaderSize = 0;
	miChunkSize = 0;
   meChunkFinished = HEADER;
	miCapHeaderSize = KEYVAL_BUFFER_SIZE;
	miCapChunkSize = KEYVAL_BUFFER_SIZE;
	mpoHeaders = moHeaderBuffer;
	mpoChunks = moChunkBuffer;
	mpcData = NULL;	
}

void CIMFHeader::Free()
{
	if (mpoHeaders != moHeaderBuffer)
		delete[] mpoHeaders;
	mpoHeaders = moHeaderBuffer;
}

int CIMFHeader::Parse(char* buf, int len, int offs)
{
	char* p;
	char* key;
	char* value;
	int nkv = miHeaderSize;
	KeyValue* keyvalue;
	int keylen;

// macro for calculating remaining length
#define remlen ((int) (len - (p - buf)))

   if (mpcData != NULL)
   {
     assert (mpcData + offs == buf);
     len += offs;
     p = buf;
     buf = mpcData;
   }
   else
   {
     mpcData = buf;
	  p = buf + offs;
   }
	
	if (meChunkFinished == HEADER)
   {
     p = SkipCrlf(p, remlen);
     if (!p)
       return -1;

     while (meChunkFinished == HEADER && p < buf + len) {
       key = p;
       if (*p && p[0] == CS_CR[0] && p[1] == CS_LF[0])
       {
         break;
       }

       p = SeekChar(p, remlen, ':');

       if (p)
       {
         keylen = (int) (p - key);

         *p++ = '\0';
         p = SkipWhitespace(p, remlen);

         value = p;
         p = SeekCrlf(p, remlen);
         if (p)
         {
           keyvalue = GetHeaders(nkv);

           keyvalue[nkv].keyStart = (int) (key - buf);
           keyvalue[nkv].keyLength = keylen;
           keyvalue[nkv].valueStart = (int) (value - buf);
           keyvalue[nkv].valueLength = (int) (p - value);
           nkv++;

           *p = '\0';
           p += 2;
         }
         else
         {
           p = key;
           break;
         }
       }
       else
       {
         p = key;
         break;
       }
     }
   }
	
	miHeaderSize = nkv;
	const char *ct;
   ct = GetField("transfer-encoding");
   if (ct != NULL && memcmp(ct, "chunked", 7) == 0)
   {
      keylen = ParseChunked(buf, len, p - buf);
      if (keylen > (p - buf))
        return keylen - offs;
      return keylen;
   }

	return (int) (p - buf);

#undef remlen	
}

const char* CIMFHeader::GetField(const char* key) const
{
	KeyValue* keyval;
	int i;
	
	if (!mpcData)
		return NULL;
	
	i = 0;
	while (i < miHeaderSize)
	{
		keyval = &mpoHeaders[i];
		if (strncasecmp(key, mpcData + keyval->keyStart, keyval->keyLength) == 0)
			return mpcData + keyval->valueStart;

		i++;
	}
	
	return NULL;
}
const char*	CIMFHeader::GetChunk(int num) const
{
  if (num >= miChunkSize)
  {
    return NULL;
  }
  return mpcData + mpoChunks[num].Start;
}

CIMFHeader::Chunk* CIMFHeader::GetChunks(int newSize)
{
	Chunk* newChunk;
	
	if (newSize < miCapChunkSize)
		return mpoChunks;
	
	miCapChunkSize *= 2;
	newChunk = new Chunk[miCapChunkSize];
	memcpy(newChunk, mpoChunks, miChunkSize * sizeof(Chunk));
	
	if (mpoChunks != moChunkBuffer)
		delete[] mpoChunks;
	
	mpoChunks = newChunk;
	
	return mpoChunks;
}

CIMFHeader::KeyValue* CIMFHeader::GetHeaders(int newSize)
{
	KeyValue* newkv;
	
	if (newSize < miCapHeaderSize)
		return mpoHeaders;
	
	miCapHeaderSize *= 2;
	newkv = new KeyValue[miCapHeaderSize];
	memcpy(newkv, mpoHeaders, miHeaderSize * sizeof(KeyValue));
	
	if (mpoHeaders != moHeaderBuffer)
		delete[] mpoHeaders;
	
	mpoHeaders = newkv;
	
	return mpoHeaders;
}


int Hex2Int(char* strlength);

int CIMFHeader::ParseChunked(char* buf, int len, int offs)
{
  int nBytes;
  char* pStart = buf + offs;    // a中存放待解码的数据
  char* pTemp;
  char strlength[10];   //一个chunk块的长度
  int  length = 0;
  Chunk* chunk;
  int nChunk = miChunkSize;
  meChunkFinished = CHUNK;
#define remlen ((int) (len - (pStart - buf)))
  do{
    pTemp = pStart;
    pStart = SkipCrlf(pStart, remlen);
    if (pStart == NULL)
    {
      pStart = pTemp;
      break;
    }

    pTemp = SeekCrlf(pStart, remlen);
    if(NULL == pTemp)
    {
      break;
    }
    length = pTemp - pStart;
    memcpy(strlength, pStart, length);
    strlength[length] = '\0';
    pStart = pTemp + 2;
    nBytes = Hex2Int(strlength); //得到一个块的长度，并转化为十进制

    if(nBytes == 0)//如果长度为0表明为最后一个chunk
    {
      meChunkFinished = CHUNKD;
      break;
    }
    //record (pStart,sizeof(char),nBytes,fp);//将nBytes长度的数据写入文件中
    chunk = GetChunks(nChunk);

    chunk[nChunk].Start = (int) (pStart - buf);
    chunk[nChunk].Length = nBytes;
    nChunk++;

    *(pStart + nBytes) = '\0';
    pStart = pStart + nBytes + 2; //跳过一个块的数据以及数据之后两个字节的结束符
  }while(true);
  miChunkSize = nChunk;
	return (int) (pStart - buf);

#undef remlen	
}

static int hex_table[]={0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,1,2,3,4,5,6,
                        7,8,9,0,0,0,0,0,0,
                        0,10,11,12,13,14,15,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,0,0,10,
                        11,12,13,14,15};

int Hex2Int(char* hex_str)
{
    char ch;
    int iret = 0;
    while(ch = *hex_str++)
    {
        iret=(iret<<4)|hex_table[ch];
    }
    return iret;
}
