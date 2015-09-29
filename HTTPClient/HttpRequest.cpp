#include <stdlib.h>
#include "HttpRequest.h"

#define CR 13
#define LF 10

void HttpRequest::Init()
{
	meState = REQUEST_LINE;
	miPos = 0;
	miContentLen = -1;
	moHeader.Init();
}

void HttpRequest::Free()
{
	moHeader.Free();
}

int HttpRequest::Parse(char* buf, int len)
{
	int reqPos;
	int headPos;
	const char* cl;
	
	if (meState == REQUEST_LINE)
	{
		reqPos = moRequestLine.Parse(buf, len, 0);
		if (reqPos < 0)
			return 0;
		
		miPos = reqPos;
		meState = HEADER;
	}
	
	if (meState == HEADER)
	{
		headPos = moHeader.Parse(buf, len, miPos);
		if (headPos < 0)
			return -1;
		if (headPos >= len)
			return -1;

		miPos = headPos;
		if (buf[miPos] == CR && buf[miPos + 1] == LF)
		{
			meState = CONTENT;
			miPos += 2;
		}
		else
			return miPos;
	}

	if (meState == CONTENT)
	{
		if (miContentLen < 0)
		{
			cl = moHeader.GetField("content-length");
			if (!cl)
				miContentLen = 0;
			else
				miContentLen = (int) strtol(cl, NULL, 10);

			miPos += miContentLen;
		}

		return miPos;
	}

	return -1;
}

int HttpRequest::Encode(char* buf, int len)
{
  return 0;
}
