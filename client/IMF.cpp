#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	miCapHeaderSize = KEYVAL_BUFFER_SIZE;
	mpoHeaders = moHeaderBuffer;
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

	mpcData = buf;
	
	p = buf + offs;
	p = SkipCrlf(p, remlen);
	if (!p)
		return -1;
	
	while (p < buf + len) {
		key = p;
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
				keyvalue = GetmpoHeaders(nkv);
				
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
	
	miHeaderSize = nkv;
	
	return (int) (p - buf);

#undef remlen	
}

const char* CIMFHeader::GetField(const char* key)
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


CIMFHeader::KeyValue* CIMFHeader::GetmpoHeaders(int newSize)
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
