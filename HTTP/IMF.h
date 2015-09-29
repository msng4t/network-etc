#ifndef IMF_H
#define IMF_H

/*
 * Internet message format
 * RFC 2822
 * Suitable for parsing HTTP, SIP and MIME headers
 */

class CIMFHeader
{
public:
	class KeyValue
	{
	public:
		int keyStart;
		int keyLength;
		int valueStart;
		int valueLength;
	};

	class RequestLine
	{
	public:
		const char*	method;
		int			methodLen;
		const char* uri;
		int			uriLen;
		const char* version;
		int			versionLen;

		int Parse(char* buf, int len, int offs);
	};

	class StatusLine
	{
	public:
		const char* version;
		int			versionLen;
		const char* code;
		int			codeLen;
		const char* reason;
		int			reasonLen;

		int Parse(char* buf, int len, int offs);
	};

public:
	enum { KEYVAL_BUFFER_SIZE = 16 };
	int				miHeaderSize;
	int				miCapHeaderSize;
	KeyValue*		mpoHeaders;
	KeyValue		moHeaderBuffer[KEYVAL_BUFFER_SIZE];
	char*			mpcData;

	CIMFHeader();
	~CIMFHeader();

	void			Init();
	void			Free();
	int				Parse(char* buf, int len, int offs);
	const char*		GetField(const char* key);

	KeyValue*		GetmpoHeaders(int newSize);
};



#endif
