#ifndef HTTP_CONTENT_HANDLER_H
#define HTTP_CONTENT_HANDLER_H
#include "Memblock.h"
#include "HttpRequest.h"
#include "HttpContentHandler.h"

class HttpFileHandler : public HttpHandler
{
public:
	HttpFileHandler(const char* docroot, const char* prefix);
	
	virtual bool	HandleRequest(mina::CMemblock* conn, const HttpRequest& request);
	
private:
	const char*		documentRoot;
	const char*		prefix;
};

#endif
