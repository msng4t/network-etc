#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HttpResponse.h"

#define CR 13
#define LF 10

void HttpResponse::Init()
{
	meState = RESPONSE_LINE;
	miPos = 0;
	miContentLen = -1;
	moHeader.Init();
}

void HttpResponse::Free()
{
	moHeader.Free();
}

int HttpResponse::Parse(char* buf, int len)
{
	int reqPos;
	int headPos;
	const char* cl;

	if (meState == RESPONSE_LINE)
	{
		reqPos = moStatusLine.Parse(buf, len, 0);
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
		if (buf[miPos + 1] == CR && buf[miPos + 2] == LF)
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
         {
				miContentLen = 0;
         }
			else
         {
				miContentLen = (int) strtol(cl, NULL, 10);
         }

			miPos += miContentLen;
		}

		return miPos;
	}

	return -1;
}

int HttpResponse::Encode(char* buf, int len)
{
  return 0;
}


