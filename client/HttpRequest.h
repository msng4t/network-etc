#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "IMF.h"

class HttpRequest
{
public:
	typedef CIMFHeader::RequestLine RequestLine;
	enum State
	{
		REQUEST_LINE,
		HEADER,
		CONTENT
	};

	CIMFHeader		moHeader;
	RequestLine		moRequestLine;
	State			meState;
	int				miPos;
	int				miContentLen;
	
	void			Init();
	void			Free();
	int				Parse(char *buf, int len);
};

#endif
