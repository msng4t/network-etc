#ifndef URL_PARAM_H
#define URL_PARAM_H
#include <string>
// helper function for parsing url strings, does *not* do url decoding!
bool UrlParam_Parse(const char* url, char sep, unsigned num, /* char** arg1, char** arg2, */...);

// this class does url decoding
class UrlParam
{
public:
	UrlParam();

	bool			Init(const char* url, char sep);

	int				GetNumParams() const;

	const char*		GetParam(int nparam) const;
	int				GetParamLen(int nparam) const;

	int				GetParamIndex(const char* name, int namelen = -1) const;
	
	bool			Get(unsigned num, /* char** */...) const;
	bool			GetNamed(const char* name, int namelen, char* bs) const;
	
private:
	const char*		url;
	char			sep;
	std::string 	params;
	std::string     buffer;
	int				numParams;
	
	bool			Parse();
	void			AddParam(const char* s, int length);

	static char		DecodeChar(const char* s);
	static char		HexToChar(char uhex, char lhex);
	static char		HexdigitToChar(char hexdigit);
};

#endif
