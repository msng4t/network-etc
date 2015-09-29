#include "Http.h"


int CHttp::DecodeStartLine(void)
{
    mina::CMemblock *block = this->mpoBlock;
    const char* start = block->GetRead();
    size_t size = block->GetSize();
    const char* cur = start;
    const char* sep;
    int len = -1;

    if (size < strlen("get x HTTP/1.1\r\n"))
        return -1;

    sep= strchr(cur, ' ');

    if (sep == NULL || sep == cur)
        return CMINA_ERROR_METHOD;

    len = sep - cur;

    switch (len)
    {
        case 3:
            {
                if (strncmp(cur, "GET", 3) == 0)
                {
                    this->muMethod = CMINA_HTTP_GET;
                    break;
                }
                if (strncmp(cur, "PUT", 3) == 0)
                {
                    this->muMethod = CMINA_HTTP_PUT;
                    break;
                }
                break;
            }
        case 4:
            {
                if (*(cur+1) == 'O')
                {
                    if (strncmp(cur, "POST", 4) == 0)
                    {
                        this->muMethod = CMINA_HTTP_POST;
                        break;
                    }
                    if (strncmp(cur, "COPY", 4) == 0)
                    {
                        this->muMethod = CMINA_HTTP_COPY;
                        break;
                    }
                    if (strncmp(cur, "MOVE", 4) == 0)
                    {
                        this->muMethod = CMINA_HTTP_MOVE;
                        break;
                    }
                    if (strncmp(cur, "LOCK", 4) == 0)
                    {
                        this->muMethod = CMINA_HTTP_LOCK;
                        break;
                    }
                }
                else
                {
                    if (strncmp(cur, "HEAD", 4) == 0)
                    {
                        this->muMethod = CMINA_HTTP_HEAD;
                        break;
                    }
                }

                break;
            }

        case 5:
            {
                if (strncmp(cur, "MKCOL", 5) == 0)
                {
                    this->muMethod = CMINA_HTTP_MKCOL;
                    break;
                }

                if (strncmp(cur, "PATCH", 5) == 0)
                {
                    this->muMethod = CMINA_HTTP_PATCH;
                    break;
                }

                if (strncmp(cur, "TRACE", 5) == 0)
                {
                    this->muMethod = CMINA_HTTP_TRACE;
                    break;
                }

                break;
            }
        case 6:
            {
                if (strncmp(cur, "DELETE", 6) == 0)
                {
                    this->muMethod = CMINA_HTTP_DELETE;
                    break;
                }

                if (strncmp(cur, "UNLOCK", 6) == 0)
                {
                    this->muMethod = CMINA_HTTP_UNLOCK;
                    break;
                }
                break;
            }

        case 7:
            {
                if (strncmp(cur, "OPTIONS", 7) == 0)
                {
                    this->muMethod = CMINA_HTTP_OPTIONS;
                }
                break;
            }
        case 8:
            {
                if (strncmp(cur, "PROPFIND", 8) == 0)
                {
                    this->muMethod = CMINA_HTTP_PROFIND;
                }
                break;
            }
        case 9:
            {
                if (strncmp(cur, "PROPPATCH", 9) == 0)
                {
                    this->muMethod = CMINA_HTTP_PROPPACHE;
                }
                break;
            }
        default:
            return CMINA_ERROR_METHOD;
    }

    cur = sep + 1;
    sep = strchr(cur, ' ');

    if (sep == NULL || sep == cur)
        return CMINA_ERROR_URI;

    this->mpcURI = strndup(cur, sep - cur);
    if (this->mpcURI == NULL)
    {
        return CMINA_ERROR_URI;
    }

    cur = sep + 1;

    if (strncmp(cur, "HTTP/", 5) != 0
            || !isdigit(*(cur + 5))
            || *(cur + 6) != '.'
            || !isdigit(*(cur +7)))
    {
        return CMINA_ERROR_VERSION;

    }

    this->muVersion = (*(cur + 5) - '0') * 10 + (*(cur + 7) - '0');

    if (this->muVersion == 9 && this->muMethod != CMINA_HTTP_GET)
    {
        return CMINA_ERROR_METHOD;
    }

    block->SetRead(cur - start + 10);

    return MINA_OK;

}

