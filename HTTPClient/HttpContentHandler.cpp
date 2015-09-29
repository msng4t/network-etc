#include "HttpContentHandler.h"
#include "HttpConsts.h"
#include "Mime.h"
#include "Log.h"
#include "HttpResponse.h"
#include "json/json.h"

#include <stdio.h>
#define MAX_MESSAGE_SIZE    4096
#define CS_CR               "\015"
#define CS_LF               "\012"
#define CS_CRLF             CS_CR CS_LF
using namespace mina;
void MakeContent(mina::CMemblock *apoReqBlock);

int ResponseHeader(const HttpRequest& request, int code, char* apcBuffer, int aiSize, const char* apcHeader);

void HttpHandler::HttpHeaderWrite(mina::CMemblock& loHeader, const char* apcHeader, size_t apcHeaderLen, const char* apcValue, size_t apcValueLen)
{
	loHeader.Write(apcHeader, apcHeaderLen);
	loHeader.Write(": ", 2);
	loHeader.Write(apcValue, apcValueLen);
	loHeader.Write(CS_CRLF, 2);
}

bool ContentHandler::HandleRequest(mina::CMemblock* response, const HttpRequest& request)
{
	char	        lacContentLen[128];
	mina::CMemblock	loHeader(1024);
	mina::CMemblock	loContent(1024 * 10);
	size_t          liSize;

//    MakeContent(&loContent);
	snprintf(lacContentLen, sizeof(lacContentLen), "%lu", loContent.GetSize());
	this->HttpHeaderWrite(loHeader,
		HTTP_HEADER_CONTENT_LENGTH, sizeof(HTTP_HEADER_CONTENT_LENGTH) - 1,
		lacContentLen, strlen(lacContentLen));

    do
    {
        liSize = this->ResponseHeader(request, HTTP_STATUS_CODE_OK,
                response->GetWrite(), response->GetDirectSpace(), loHeader.GetRead());
        if (liSize > response->GetDirectSpace())
        {
            if (response->GetSpace() > liSize)
            {
                response->Shrink();
            }
            else
            {
                response->Resize(response->GetSize() + liSize + 1);
            }
            continue;
        }

    } while (false);
    response->SetWrite(liSize);
    response->Write(loContent.GetRead(), loContent.GetSize());

	return true;
}

bool ContentHandler::HandleResponse(mina::CMemblock* response, const HttpResponse& request)
{
  int chunknum = 0;
  const char* lpcContent;
  Json::Reader loJsonReader;
  while ((lpcContent = request.moHeader.GetChunk(chunknum)) != NULL)
  {
    Json::Value loRoot;
    if(loJsonReader.parse(lpcContent, loRoot))
    {
      if(!loRoot["action"].isNull())
      {
        printf("action: %s\n", loRoot["action"].asString().c_str());    ///读取元素
        printf("node:\n");    ///读取元素
        printf("\tkey:           %s\n", loRoot["node"]["key"].asString().c_str());    ///读取元素
        printf("\tvalue:         %s\n", loRoot["node"]["value"].asString().c_str());    ///读取元素
        printf("\tmodifiedIndex: %s\n", loRoot["node"]["modifiedIndex"].asString().c_str());    ///读取元素
        printf("\tcreatedIndex:  %s\n", loRoot["node"]["createdIndex"].asString().c_str());    ///读取元素

        printf("prevNode:\n");    ///读取元素
        printf("\tkey:           %s\n", loRoot["prevNode"]["key"].asString().c_str());    ///读取元素
        printf("\tvalue:         %s\n", loRoot["prevNode"]["value"].asString().c_str());    ///读取元素
        printf("\tmodifiedIndex: %s\n", loRoot["prevNode"]["modifiedIndex"].asString().c_str());    ///读取元素
        printf("\tcreatedIndex:  %s\n", loRoot["prevNode"]["createdIndex"].asString().c_str());    ///读取元素
      }
    }
    chunknum++;
  }
  return true;
}

void MakeContent(mina::CMemblock *apoReqBlock)
{
    ssize_t liSize = 0;
    const char* lpcContent =
            "<html>"
            "<head><title>ttt</title></head>"
            "<body>"
            ""
            "<form  action=\"asdocs\" method=\"post\">"
            "name:"
            "<input type=\"text\" name=\"yourname\">"
            "<input type=\"text\" name=\"password\">"
            "<input type=\"submit\" value=\"submit\">"
            "</form>"
            ""
            "</body>"
            "</html>\0";
    do{
        liSize = apoReqBlock->Write(lpcContent, strlen(lpcContent) + 1);
        if (apoReqBlock->Write(lpcContent, strlen(lpcContent) + 1) < 0)
        {
            LOG_ERROR("Write failed.");
            apoReqBlock->Resize(apoReqBlock->GetCapacity());
        }
    }while (liSize < 0);
}

int HttpHandler::ResponseHeader(const HttpRequest& request, int code, char* apcBuffer, int aiSize, const char* apcHeader)
{
    int liSize;
    const char *version = request.moRequestLine.version;
    liSize = snprintf(apcBuffer, aiSize,
            "%s %d %s" CS_CRLF
            "Cache-Control: no-cache" CS_CRLF
            "%s"
            "%s"
            CS_CRLF ,
            version == NULL ? "HTTP/1.1" : version,
            code,
            code == HTTP_STATUS_CODE_OK ? "OK" : "",
            "Connection: close" CS_CRLF,
            apcHeader ? apcHeader : "");

     return liSize;
}
