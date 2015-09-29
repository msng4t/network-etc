#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "IMF.h"

class HttpResponse
{
public:
	typedef CIMFHeader::StatusLine StatusLine;
	enum State
	{
		RESPONSE_LINE,
		HEADER,
		CONTENT
	};

	CIMFHeader		moHeader;
	StatusLine     moStatusLine;
	State			meState;
	int				miPos;
	int				miContentLen;
	
	void			Init();
	void			Free();
	int			Parse(char *buf, int len);
   int         Encode(char* buf, int len);
};

#endif
