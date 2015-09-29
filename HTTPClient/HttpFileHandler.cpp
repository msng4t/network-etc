#include "HttpFileHandler.h"
#include "HttpConsts.h"
#include "Mime.h"

#include <stdio.h>
#define MAX_MESSAGE_SIZE    4096
#define CS_CR               "\015"
#define CS_LF               "\012"
#define CS_CRLF             CS_CR CS_LF
#define MSG_FAIL            "Unable to process your request at this time"
#define MSG_NOT_FOUND       "Not found"
#define PARAMSEP            ','

HttpFileHandler::HttpFileHandler(const char* docroot, const char* prefix_)
{
	documentRoot = docroot;
	prefix = prefix_;
}

bool HttpFileHandler::HandleRequest(mina::CMemblock* response, const HttpRequest& request)
{
	char    path[128];
	char	tmp[128];
	mina::CMemblock	ha;
	char			buf[128 * 1024];
	FILE*			fp;
	size_t			nread;
	int             lsize;
	const char*		mimeType;
	
	if (strncmp(request.moRequestLine.uri, prefix, strlen(prefix)))
		return false;
	
	snprintf(path, sizeof(path), "%s%s", documentRoot, request.moRequestLine.uri);
	
	mimeType = MimeTypeFromExtension(strrchr(path, '.'));
	
	fp = fopen(path, "rb");
	if (!fp)
		return false;
	
	fseek(fp, 0L, SEEK_END);
	lsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	this->HttpHeaderWrite(ha,
		HTTP_HEADER_CONTENT_TYPE, sizeof(HTTP_HEADER_CONTENT_TYPE) - 1,
		mimeType, strlen(mimeType));
	
	snprintf(tmp, sizeof(tmp), "%d", lsize);
	HttpHeaderWrite(ha,
		HTTP_HEADER_CONTENT_LENGTH, sizeof(HTTP_HEADER_CONTENT_LENGTH) - 1,
		tmp, strlen(tmp));

	// zero-terminate
	ha.Write("", 1);
	
	lsize = ResponseHeader(request, HTTP_STATUS_CODE_OK, ha.GetWrite(), ha.GetDirectSpace(), NULL);
    if (lsize > 0)
    {
        ha.SetWrite(lsize);
    }

	
	while (true)
	{
		nread = fread(buf, 1, sizeof(buf), fp);
		if (feof(fp))
		{
			fclose(fp);
			response->Write(buf, nread);
			break;
		}
		if (ferror(fp))
		{
			fclose(fp);
			break;
		}
		
		response->Write(buf, nread);
	}

	return true;
}
