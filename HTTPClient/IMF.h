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

      int Encode(char* buf, int len);
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

   class Chunk
   {
     public:
     int Start;
     int Length;
   };

public:
	enum { KEYVAL_BUFFER_SIZE = 16 };
	enum State{ HEADER, CHUNK, CHUNKD };
	int				miHeaderSize;
	int				miCapHeaderSize;
	int				miChunkSize;
	State          meChunkFinished;
	int				miCapChunkSize;

	KeyValue*		mpoHeaders;
   Chunk*         mpoChunks;

	KeyValue		moHeaderBuffer[KEYVAL_BUFFER_SIZE];
   Chunk       moChunkBuffer[KEYVAL_BUFFER_SIZE];
	char*			mpcData;

	CIMFHeader();
	~CIMFHeader();

	bool        ChunkFinished() { return meChunkFinished == CHUNKD; }
	void			Init();
	void			Free();
	int		   Parse(char* buf, int len, int offs);
   int         ParseChunked(char* buf, int len, int offs);
	const char*	GetField(const char* key) const;
	const char*	GetChunk(int num) const;

	KeyValue*	GetHeaders(int newSize);
	Chunk*		GetChunks(int newSize);
};



#endif
