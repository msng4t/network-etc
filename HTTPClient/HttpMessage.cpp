#include<stdio.h>
#include "HttpMessage.h"

bool checkSPHT_CRLF_COMMA(char c)
{ 
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n'|| c == ',') 
        return true;
    return false;
}

///////////////////////////////////////////////////////////////////////////////
CHttpMessage::CHttpMessage(int aiType)
    :moVersion("1.1")
    ,mbContentLengthFlag(true)
    ,mbTransferEncodeFlag(false)
    ,miMsgType(aiType)
{
}



CHttpMessage::~CHttpMessage(){

}

void CHttpMessage::SetMessageID(uint64_t auMsgId)
{
    //FIXME  what would be the message id of http?
    //return 0;
}

uint64_t CHttpMessage::GetMessageID()
{
    //FIXME  what would be the message id of http?
    return 0;
}

void CHttpMessage::AddHeader(std::string const& aoName, std::string const& aoValue, char acSep)
{
    // Search in the filedlist 
    StrStrMMap::iterator lpPos;

    if (aoName.empty())
        return;

    lpPos = moFieldList.find(aoName);

    // the filed exists
    if (lpPos != moFieldList.end())
    {   
        //add nothing
        if (aoValue.empty())
            return;

        //if the exist filed value not null
        if (lpPos->second.size()) 
        {
            lpPos->second += acSep;
            lpPos->second += aoValue;
            return;
        }
    }

    // the filed is new or the value is NULL
    moFieldList[aoName] = aoValue;
}


void CHttpMessage::DelHeader(std::string const& aoName)
{
    // Search in the filedlist 
    StrStrMMap::iterator lpPos;

    if (aoName.empty())
        return;

    lpPos = moFieldList.find(aoName);

    // erase the header if found
    if (lpPos != moFieldList.end())
    {
        moFieldList.erase(aoName);
    }

}

std::string CHttpMessage::GetHeader(std::string const& aoName)
{
    StrStrMMap::iterator lpPos;
    std::string loStr; //the empty string ready to return

    if (aoName.empty())
    {   
        return loStr;
    }

    lpPos = moFieldList.find(aoName);

    //return the value if found the head
    if (lpPos != moFieldList.end())
    {
        return lpPos->second;
    }

    return loStr;
}

StrStrMMap const& CHttpMessage::GetHeaderList(void)
{
    return moFieldList;
}

void CHttpMessage::SetHeader(std::string const& aoName, std::string const& aoValue)
{

    if (aoName.empty())
    {   
        return;
    }

    moFieldList[aoName] = aoValue;
}

void CHttpMessage::ClearHeader()
{
    moFieldList.clear();
}

ssize_t CHttpMessage::AddBody(std::string const& aoData)
{
    ssize_t liLen = moEntity.size();

    moEntity += aoData;
    liLen = moEntity.size() - liLen;

    return liLen;
}


ssize_t CHttpMessage::AddBody(const char* apcBuf, size_t aiBufLen)
{
    ssize_t liLen = moEntity.size();

    if (apcBuf == NULL)
        return -1;

    moEntity.append(apcBuf, apcBuf+aiBufLen);

    liLen = moEntity.size() - liLen;

    return liLen;
}

std::string const& CHttpMessage::GetBody()
{
        return  moEntity;
}


ssize_t CHttpMessage::SetBody(std::string const& aoData)
{
    ssize_t liLen = 0;

    moEntity = aoData;
    liLen = moEntity.size();

    return liLen;
}


ssize_t CHttpMessage::SetBody(const char* apcBuf, size_t aiBufLen)
{
    ssize_t liLen = 0;

    ClearBody();
    liLen = AddBody(apcBuf, aiBufLen);

    return liLen; 
}

void CHttpMessage::ClearBody()
{
    moEntity.erase();
}


size_t CHttpMessage::GetBodySize()
{
    return moEntity.size();
}


std::string const& CHttpMessage::GetHttpVersion(){
    return moVersion;
}


void CHttpMessage::SetHttpVersion(std::string const& aoVersion){
    moVersion = aoVersion;
}


/////////////////////////////Encoder
int CHttpMessage::EncodeHeader(CMemblock* apoBuffer)
{
    StrStrMMap::iterator loIter;
    char * lpcCur = NULL;
    char * lpcStart = NULL;
    char* lpcEnd = NULL;
    size_t liBufSize = 0;
    ssize_t liRet;

    lpcStart = apoBuffer->GetWrite();
    lpcCur = lpcStart;

    if (NULL == lpcCur)
        return HTTP_ERROR_INVAL;

    liBufSize = apoBuffer->GetSpace();
    lpcEnd = lpcCur + liBufSize;

    //calc the content len automatically
    if (GetAutoCalcContentLengthFlag())
    {
        size_t liSize = this->GetBodySize();
        char lcaBuf[128]; //128bit for len should enough

        this->DelHeader(HTTP_CONTENT_LENGTH);

        if (liSize > 0) 
        {
            memset(lcaBuf, 0, sizeof(lcaBuf));
            snprintf(lcaBuf, sizeof(lcaBuf), "%lu", liSize);
            this->AddHeader(HTTP_CONTENT_LENGTH, lcaBuf);
        }
    }

    for (loIter = this->moFieldList.begin(); loIter != this->moFieldList.end(); ++loIter) 
    {
        liRet = snprintf(lpcCur, lpcEnd - lpcCur, "%s:%s\r\n", loIter->first.c_str(), loIter->second.c_str());

        if (liRet > (lpcEnd - lpcCur - 1)) 
            return HTTP_ERROROOBIG;

        lpcCur += liRet; 
    }

    if ((lpcEnd - lpcCur) < 3)
        return HTTP_ERROROOBIG;

    memcpy(lpcCur, "\r\n\0", 3);

    lpcCur += 2;

    apoBuffer->SetWrite(lpcCur);

    return 0;
}

int CHttpMessage::EncodeBody(CMemblock* apoBuffer)
{
    char * lpcBuf = NULL;

    lpcBuf = apoBuffer->GetWrite();

    if (NULL == lpcBuf)
        return HTTP_ERROR_INVAL;

    return apoBuffer->Write(this->GetBody().data(), this->GetBody().size());
}

void CHttpMessage::SetAutoCalcContentLength(bool abFlag)
{
    mbContentLengthFlag = abFlag;
}

void CHttpMessage::SetAutoTransferEncoding(bool abFlag)
{
    mbTransferEncodeFlag = abFlag;
}

bool CHttpMessage::GetAutoCalcContentLengthFlag()
{
    return mbContentLengthFlag;
}

bool CHttpMessage::GetAutoTransferEncodingFlag()
{
    return mbTransferEncodeFlag;
}

int CHttpMessage::EncodeMessage(CMemblock* apoBuffer)
{
    char* lpcStart = apoBuffer->GetWrite();
    int liRet = 0;

    this->SaveMBWrite(apoBuffer);

    if (lpcStart == NULL)
        return HTTP_ERROR_INVAL;

    liRet = EncodeStartLine(apoBuffer);
    if (liRet <0) goto ENCODE_FAILED; 

    liRet = EncodeHeader(apoBuffer);
    if (liRet <0) goto ENCODE_FAILED; 

    liRet = EncodeBody(apoBuffer);
    if (liRet <0) goto ENCODE_FAILED; 

    return apoBuffer->GetWrite() - lpcStart;

ENCODE_FAILED:
    this->RestoreMBWrite(apoBuffer);
    return liRet;
}

void CHttpMessage::SaveMBWrite(CMemblock* apoBuffer)
{
    this->mpcMBWrite = apoBuffer->GetWrite();
}

void CHttpMessage::RestoreMBWrite(CMemblock* apoBuffer)
{
    apoBuffer->SetWrite(this->mpcMBWrite);
}

void CHttpMessage::SaveMBRead(CMemblock* apoBuffer)
{
    this->mpcMBRead = apoBuffer->GetRead();
}

void CHttpMessage::RestoreMBRead(CMemblock* apoBuffer)
{
    apoBuffer->SetRead(this->mpcMBRead);
}

int CHttpMessage::GetMsgType()
{
    return this->miMsgType;
}

void CHttpMessage::SetErrMsg(std::string const & aoErrMsg)
{
}

const char* CHttpMessage::GetErrMsg()
{
    return NULL;
}

void CHttpMessage::SetErrCode(int aiErrCode)
{
}

int CHttpMessage::GetErrCode()
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

CHttpRequest::CHttpRequest()
    :CHttpMessage(MESSAGEYPE_HTTP_REQUEST)
{
}

CHttpRequest::~CHttpRequest()
{
}

std::string const& CHttpRequest::GetMethod()
{
    return  moMethod;
}

std::string const& CHttpRequest::GetRequestURI()
{
    return  moUri;
}

void CHttpRequest::SetMethod(std::string const& aoMethod)
{
    moMethod = aoMethod;
}

void CHttpRequest::SetRequestURI(std::string const& aoUri)
{
    moUri = aoUri;
}

int CHttpRequest::EncodeStartLine(CMemblock* apoBuffer)
{
    ssize_t liRet;
    char * lpcBuf = NULL;
    size_t liBufSize = 0;

    lpcBuf = apoBuffer->GetWrite();

    if (NULL == lpcBuf)
        return HTTP_ERROR_INVAL;

    liBufSize = apoBuffer->GetSpace();

    liRet = snprintf(lpcBuf, liBufSize, "%s %s HTTP/%s\r\n", 
            this->GetMethod().c_str(), this->GetRequestURI().c_str(), this->GetHttpVersion().c_str());

    if (liRet < 0 || size_t(liRet + 1) > liBufSize)
        return HTTP_ERROROOBIG;

    apoBuffer->SetWrite(lpcBuf + liRet);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////

CHttpResponse::CHttpResponse()
    :CHttpMessage(MESSAGEYPE_HTTP_RESPONSE)
    ,muStatus(0)
{
}

CHttpResponse::~CHttpResponse(){

}

std::string const& CHttpResponse::GetReasonPhrase()
{
    return  moReason;
}

uint32_t CHttpResponse::GetStatusCode()
{
    return  muStatus;
}

void CHttpResponse::SetReasonPhrase(std::string const& aoReasonPhrase)
{
    moReason = aoReasonPhrase;
}

void CHttpResponse::SetStatusCode(uint32_t aiStatus)
{
    muStatus = aiStatus;
}

int CHttpResponse::EncodeStartLine(CMemblock* apoBuffer)
{
    ssize_t liRet;
    char * lpcBuf = NULL;
    size_t liBufSize = 0;

    lpcBuf = apoBuffer->GetWrite();

    if (NULL == lpcBuf)
        return HTTP_ERROR_INVAL;

    liBufSize = apoBuffer->GetSpace();

    liRet = snprintf(
        lpcBuf,
        liBufSize,
        "HTTP/%s %03u %s\r\n",
        this->GetHttpVersion().c_str(),
        this->GetStatusCode() % 1000, 
        this->GetReasonPhrase().c_str()
    );

    if (liRet < 0 || size_t(liRet + 1) > liBufSize)
        return HTTP_ERROROOBIG;

    apoBuffer->SetWrite(lpcBuf + liRet);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

CHttpInvalid::CHttpInvalid()
    :CHttpMessage(MESSAGEYPE_HTTP_INVALID)
{
}

int CHttpInvalid::EncodeStartLine(CMemblock* apoBuffer)
{
    return HTTP_ERROR_INVAL;
}

void CHttpInvalid::SetErrMsg(std::string const & aoErrMsg)
{
    this->moErrMsg = aoErrMsg;
}

const char* CHttpInvalid::GetErrMsg()
{
    return this->moErrMsg.c_str();
}

void CHttpInvalid::SetErrCode(int aiErrCode)
{
    this->miErrCode = aiErrCode;
}

int CHttpInvalid::GetErrCode()
{
    return this->miErrCode;
}
