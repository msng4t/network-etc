/**
 * @file HttpMessage.h 
 * @author  $Author: hezk $
 * @date    $Date: 2009/12/15 03:40:15 $
 * @version $Revision: 1.1 $
 */
#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <string>
#include <list>
#include <map>
#include "Memblock.h"

using namespace mina;
/**
 * typedef std::map<std::string, std::string> StrStrMMap;
 */
typedef std::map<std::string, std::string> StrStrMMap;

//size_t const HTTP_MAX_LINE_LENGTH = 1024;
//size_t const HTTP_MAX_HEADER_LENGTH = 4096;
//
////define message type string
const int MESSAGEYPE_HTTP_INVALID           = 1;
const int MESSAGEYPE_HTTP_REQUEST           = 2;
const int MESSAGEYPE_HTTP_RESPONSE          = 3;
//
///// errors in return value
#define HTTP_ERROR_INVAL         (-90601)    ///< invalid args, (e.g. NULL pointer)
//#define HTTP_ERROR_NOTFOUND      (-90602)    ///< header field not found (CHttpMessage)
#define HTTP_ERROROOBIG        (-90603)    ///< buffer is too small
//#define HTTP_ERROR_EOL           (-90604)    ///< End-Of-Line not found
//#define HTTP_ERROR_STARTLINE     (-90605)    ///< other error in start-line
//#define HTTP_ERROR_BODY          (-90606)    ///< don't know how to recv message-body
//#define HTTP_ERROR_size_t          (-90607)    ///< invalid Content-Length or chunk-size
//#define HTTP_ERROR_SOCKET        (-90608)    ///< socket error
//
//#define HTTP_ERROR_VERSION       (-90611)    ///< invalid version in start-line
//#define HTTP_ERROR_METHOD        (-90612)    ///< invalid method in request-line 
//#define HTTP_ERROR_URI           (-90613)    ///< invalid uri in request-line
//#define HTTP_ERROR_STATUS        (-90614)    ///< invalid statut in response-line
//#define HTTP_ERROR_FIELDNAME     (-90615)    ///< invalid field name
//#define HTTP_ERROR_FIELDVALUE    (-90616)    ///< invalid field value
//#define HTTP_ERROR_FIELDSEP      (-90617)    ///< ':' not found in feild line
//#define HTTP_ERROR_LOCATION      (-90618)    ///< Location not found to redirect
//#define HTTP_ERROR_REDIRECT      (-90619)    ///< too many redirection
//#define HTTP_ERRORYPE          (-90620)    ///< message type is incorrect
//#define HTTP_ERROR_BUF_NOT_ENOUGH (-90621)    ///< message type is incorrect
//#define HTTP_ERROR_CONTENT_LENGTH (-90622)    ///< invalid content length
//

/****************************************************************************
 * http methods (RFC 2616)
 ****************************************************************************/
#define HTTP_METHOD_GET      "GET"
#define HTTP_METHOD_HEAD     "HEAD"
#define HTTP_METHOD_POST     "POST"
#define HTTP_METHOD_PUT      "PUT"
#define HTTP_METHOD_DELETE   "DELETE"
#define HTTP_METHODRACE    "TRACE"
#define HTTP_METHOD_CONNECT  "CONNECT"

/****************************************************************************
 * general header fields (RFC 2616)
 ****************************************************************************/
#define HTTP_CACHE_CONTROL         "Cache-Control"
#define HTTP_CONNECTION            "Connection"
#define HTTP_DATE                  "Date"
#define HTTP_PRAGMA                "Pragma"
#define HTTPRAILER               "Trailer"
#define HTTPRANSFER_ENCODING     "Transfer-Encoding"
#define HTTP_UPGRADE               "Upgrade"
#define HTTP_VIA                   "Via"
#define HTTP_WARNING               "Warning"

/****************************************************************************
 * request header fields (RFC 2616)
 ****************************************************************************/
#define HTTP_ACCEPT                "Accept"
#define HTTP_ACCEPT_CHARSET        "Accept-Charset"
#define HTTP_ACCEPT_ENCODING       "Accept-Encoding"
#define HTTP_ACCEPT_LANGUAGE       "Accept-Language"
#define HTTP_AUTHORIZATION         "Authorization"
#define HTTP_EXPECT                "Expect"
#define HTTP_FROM                  "From"
#define HTTP_HOST                  "Host"
#define HTTP_IF_MATCH              "If-Match"
#define HTTP_IF_MODIFIED_SINCE     "If-Modified-Sinc"
#define HTTP_IF_NONE_MATCH         "If-None-Match"
#define HTTP_IF_RANGE              "If-Range"
#define HTTP_IF_UNMODIFIED_SINCE   "If-Unmodified-Sinc"
#define HTTP_MAX_FORWARDS          "Max-Forwards"
#define HTTP_PROXY_AUTHORIZATION   "Proxy-Authorization"
#define HTTP_RANGE                 "Range"
#define HTTP_REFFER                "Reffer"
#define HTTPE                    "TE"
#define HTTP_USER_AGENT            "User-Agent"

/****************************************************************************
 * response header fields (RFC 2616)
 ****************************************************************************/
#define HTTP_ACCEPT_RANGES         "Accept-Ranges"
#define HTTP_AGE                   "Age"
#define HTTP_ETAG                  "ETag"
#define HTTP_LOCATION              "Location"
#define HTTP_PROXY_AUTHENTICATE    "Proxy-Authenticate"
#define HTTP_RETRY_AFTER           "Retry-After"
#define HTTP_SERVER                "Server"
#define HTTP_VARY                  "Vary"
#define HTTP_WWW_AUTHENTICATE      "WWW-Authenticate"

/****************************************************************************
 * entity header fields (RFC 2616)
 ****************************************************************************/
#define HTTP_ALLOW                 "Allow"
#define HTTP_CONTENT_ENCODING      "Content-Encoding"
#define HTTP_CONTENT_LANGUAGE      "Content-Language"
#define HTTP_CONTENT_LENGTH        "Content-Length"
#define HTTP_CONTENT_LOCATION      "Content-Location"
#define HTTP_CONTENT_MD5           "Content-MD5"
#define HTTP_CONTENT_RANGE         "Content-Range"
#define HTTP_CONTENTYPE          "Content-Type"
#define HTTP_EXPIRES               "Expires"
#define HTTP_LAST_MODIFIED         "Last-Modified"


/****************************************************************************
 * status codes (RFC 2616)
 ****************************************************************************/
enum
{
    HTTP_CONTINUE                         = 100,
    HTTP_SWITCHING_PROTOCOLS              = 101,

    HTTP_OK                               = 200,
    HTTP_CREATED                          = 201,
    HTTP_ACCEPTED                         = 202,
    HTTP_NON_AUTHORITATIVE_INFORMATION    = 203,
    HTTP_NO_CONTENT                       = 204,
    HTTP_RESET_CONTENT                    = 205,
    HTTP_PARTIAL_CONTENT                  = 206,

    HTTP_MULTIPLE_CHOICES                 = 300,
    HTTP_MOVED_PERMANENTLY                = 301,
    HTTP_FOUND                            = 302,
    HTTP_SEE_OTHER                        = 303,
    HTTP_NOT_MODIFIED                     = 304,
    HTTP_USE_PROXY                        = 305,
    HTTPEMPORARY_REDIRECT               = 307,

    HTTP_BAD_REQUEST                      = 400,
    HTTP_BAD_UNAUTHORIZED                 = 401,
    HTTP_PAYMENT_REQUIRED                 = 402,
    HTTP_FORBIDDEN                        = 403,
    HTTP_NOT_FOUND                        = 404,
    HTTP_METHOD_NOT_ALLOWED               = 405,
    HTTP_NOT_ACCEPTABLE                   = 406,
    HTTP_PROXY_AUTHENTICATION_REQUIRED    = 407,
    HTTP_REQUESTIMEOUT                  = 408,
    HTTP_CONFLICT                         = 409,
    HTTP_GONE                             = 410,
    HTTP_LENGTH_REQUIRED                  = 411,
    HTTP_PRECONDITION_FAILED              = 412,
    HTTP_REQUEST_ENTITYOO_LARGE         = 413,
    HTTP_REQUEST_URIOO_LONG             = 414,
    HTTP_UNSUPPORTED_MEDIAYPE           = 415,
    HTTP_REQUESTED_RANGE_NOT_SATISFIABLE  = 416,
    HTTP_EXPECTATION_FAILED               = 417,

    HTTP_INTERNAL_SERVER_ERROR            = 500,
    HTTP_NOT_IMPLEMENTED                  = 501,
    HTTP_BAD_GATEWAY                      = 502,
    HTTP_SERVICE_UNAVAILABLE              = 503,
    HTTP_GATEWAYIMEOUT                  = 504,
    HTTP_HTTP_VERSION_NOT_SUPPORTED       = 505

};


/**
 * Http message class 
 */
class CHttpMessage
{
public:
    /**
     * Constructor
     */
    CHttpMessage(int aiType);

    /**
     * Destructor.
     */
    virtual ~CHttpMessage();

    virtual void SetMessageID(uint64_t auMsgId);

    virtual uint64_t GetMessageID();

    /**
     * Add content to a message body.
     *
     * @param[in] aoData  string add to the body.
     *
     * @return the size_t add to message body.
     */
    ssize_t AddBody(std::string const& aoData);

    /**
     * Add content to message body.
     *
     * @param[in] apcBuf  pointer of the add-content buf.
     * @param[in] aiBufLen  size_t of the content.
     *
     * @return the size_t add to message body.
     */
    ssize_t AddBody(const char* apcBuf, size_t aiBufLen);

    /**
     * Get the message body content.
     *
     * @return the message body content in string format.
     */
    std::string const& GetBody();

    /**
     * Set message body content. 
     *
     * @param[in] aoData  string set to the body.
     *
     * @return the size_t of message body content.
     */
    ssize_t SetBody(std::string const& aoData);

    /**
     * Set message body content. 
     *
     * @param[in] apcBuf  pointer of the add-content buf.
     * @param[in] aiBufLen  size_t of the content.
     *
     * @return the size_t of message body content.
     */
    ssize_t SetBody(const char* apcBuf, size_t aiBufLen);

    /**
     * Clear message body content. 
     */
    void ClearBody();

    /**
     * Get the size_t of message body content.
     */
    size_t GetBodySize();

    /**
     * Add header filed to message. If the header filed exist, append comma to 
     * the end of the original value, then append the new value.
     *
     * @param[in] aoName  Header filed.
     * @param[in] aoValue  Header Value.
     * @param[in] acSep  Seperater, muti-line header value would need a 
     *                   space. 
     */
    void AddHeader(std::string const& aoName, std::string const& aoValue, char acSep = ',');

    /**
     * Get the value of a specified header filed.
     *
     * @param[in] aoName  Header filed.
     * 
     * @return the value of the header field.
     */
    std::string GetHeader(std::string const& aoName);

    /**
     * Get the list of all the header filed.
     *
     * @return the list of all header field.
     */
    StrStrMMap const& GetHeaderList(void);

    /**
     * Delete a specified header field.
     *
     * @return the list of all header field.
     */
    void DelHeader(std::string const& aoName);

    /**
     * Set a specified header field's value
     *
     * @param[in] aoName  Header filed.
     * @param[in] aoValue  Header Value.
     *
     */
    void SetHeader(std::string const& aoName, std::string const& aoValue);

    /**
     * Clear the header list.
     *
     */
    void ClearHeader();

    /**
     * Set the Http version.
     *
     * @param[in] aoVersion  Http version.
     */
    void SetHttpVersion(std::string const& aoVersion);

    /**
     * Get the Http version.
     *
     * @return  Http version.
     */
    std::string const& GetHttpVersion();

    
////////////////////////////////////////////////////////////////////////////////
    /**
     * Encode the Http header from CHttpMessage to memory block, 
     * Content-Length will be filled automatically when 
     * mbContentLengthFlag is true.
     *
     * @param[in] apoBuffer  pointer to a MemoryBlock. Header field in 
     *                       this message will encode and append to 
     *                       memory block.
     *
     * @retval =0 success
     * @retval <0 failure, indicate the error type.
     */
	int EncodeHeader(CMemblock* apoBuffer);

    /**
     * Encode the Http body from CHttpMessage to memory block. 
     *
     * @param[in] apoMsg  pointer to a CHttpMessage. Body in CHttpMessage 
     *                    will encode and append to memory block. 
     *
     * @retval =0 success
     * @retval <0 failure, indicate the error type.
     */
	int EncodeBody(CMemblock* apoBuffer);

    /**
     * Encode the Http request line from CHttpMessage to memory block.
     *
     * @param[in] apoMsg  pointer to a CHttpMessage. Body in CHttpMessage
     *                    will encode and append to memory block. 
     *
     * @retval =0 success
     * @retval <0 failure, indicate the error type.
     */
	virtual int EncodeStartLine(CMemblock* apoBuffer) = 0;

    /**
     * Set the flag to calculate the content length when encoding header.
     *
     * @param[in] abFlag, bool type to set the flag. 
     */
    void SetAutoCalcContentLength(bool abFlag);

    /**
     * Set the flag to chunk the entity when encoding body.
     *
     * @param[in] abFlag, bool type to set the flag. 
     */
    void SetAutoTransferEncoding(bool abFlag);

    /**
     * Get the flag to calculate the content length when encoding header.
     *
     * @return true of false.
     */
    bool GetAutoCalcContentLengthFlag();

    /**
     * Get the flag to chunk the entity when encoding body.
     *
     * @return true of false.
     */
    bool GetAutoTransferEncodingFlag();

    /**
     * Encode a Http message.
     *
     * The encoded bytes should be appended to apoBuffer.
     *
     * @param[in] apoBuffer  pointer to a Memoryblock which to be filled.
     *
     * @retval  success, 
     * @retval <0 failure, the return value is difference from protocols.
     */
    int EncodeMessage(CMemblock* apoBuffer);

    /**
     * Save the write  pointer of memory block in case of the encode fail.
     */
    void SaveMBWrite(CMemblock* apoBuffer);

    /**
     * Restore the write pointer of memory block when the encode fail.
     */
    void RestoreMBWrite(CMemblock* apoBuffer);

////////////////////////////////////////////////////////////////////////////////
    /**
     * Save the read pointer of memory block in case of the decode fail.
     */
    void SaveMBRead(CMemblock* apoBuffer);

    /**
     * Restore the read pointer of memory block when the decode fail.
     */
    void RestoreMBRead(CMemblock* apoBuffer);

    int GetMsgType();

    virtual void SetErrMsg(std::string const & aoErrMsg);

    virtual const char* GetErrMsg();

    virtual void SetErrCode(int aiErrCode);

    virtual int GetErrCode();

///////////////////////////////////////////////////////////////////////////////


protected:     
    StrStrMMap      moFieldList; //header field list
    std::string     moEntity; //body string
    std::string     moVersion; //http version

    bool            mbContentLengthFlag; //default true
    bool            mbTransferEncodeFlag; //default false
    char*           mpcMBWrite; //memory block write pt
    char*           mpcMBRead; //memory block read ptr

    int       miMsgType;
};


class CHttpRequest : public CHttpMessage
{

public:
    /**
     * Constructor.
     */
    CHttpRequest();

    /**
     * Destructor.
     */
    virtual ~CHttpRequest();

    /**
     * Get request method.
     */
    std::string const& GetMethod();

    /**
     * Get request URI.
     */
    std::string const& GetRequestURI();

    /**
     * Set request method.
     *
     * @param[in] aoMethod  request method.
     */
    void SetMethod(std::string const& aoMethod);

    /**
     * Set request URI.
     *
     * @param[in] aoUri  request URI.
     */
    void SetRequestURI(std::string const& aoUri);

	virtual int EncodeStartLine(CMemblock* apoBuffer);

protected:
    std::string moMethod; //request method
    std::string moUri; //request URI

};



class CHttpResponse : public CHttpMessage
{
public:
    /**
     * Constructor.
     */
    CHttpResponse();

    /**
     * Destructor.
     */
    virtual ~CHttpResponse();

    /**
     * Get response reason phrase.
     *
     * @return response reason phrase.
     */
    std::string const& GetReasonPhrase();

    /**
     * Get response status code.
     *
     * @return response status code.
     */
    uint32_t GetStatusCode();

    /**
     * Set response reason phrase.
     *
     * @param[in] aoReasonPhrase  response reason phrase.
     */
    void SetReasonPhrase(std::string const& aoReasonPhrase);

    /**
     * Set response status code.
     *
     * @param[in] aiStatus  response status code.
     */
    void SetStatusCode(uint32_t aiStatus);

	virtual int EncodeStartLine(CMemblock* apoBuffer);

protected:
    uint32_t    muStatus; //response status code
    std::string     moReason; //response reason phrase
};


class CHttpInvalid : public CHttpMessage
{
public:

    CHttpInvalid();

	virtual int EncodeStartLine(CMemblock* apoBuffer);

    virtual void SetErrMsg(std::string const & aoErrMsg);

    virtual const char* GetErrMsg();

    virtual void SetErrCode(int aiErrCode);

    virtual int GetErrCode();

private:
    std::string moErrMsg;
    int miErrCode;
};


#endif//HTTP_MESSAGE_H
