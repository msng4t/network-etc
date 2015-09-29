#ifndef HTTP_FILE_HANDLER_H
#define HTTP_FILE_HANDLER_H
#include "Memblock.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpRequest;
class HttpResponse;

class HttpHandler
{
public:
	virtual ~HttpHandler() {}
	virtual bool HandleRequest(mina::CMemblock* conn, const HttpRequest& request) = 0;
   virtual bool HandleResponse(mina::CMemblock* response, const HttpResponse& request) = 0;

    void HttpHeaderWrite(mina::CMemblock& loHeader, const char* apcHeader, size_t apcHeaderLen, const char* apcValue, size_t apcValueLen);
    int ResponseHeader(const HttpRequest& request, int code, char* apcBuffer, int aiSize, const char* apcHeader);
	HttpHandler*	nextHttpHandler;
};

class ContentHandler : public HttpHandler
{
public:
	virtual bool	HandleRequest(mina::CMemblock* conn, const HttpRequest& request);
   virtual bool HandleResponse(mina::CMemblock* response, const HttpResponse& request);

};

#endif
