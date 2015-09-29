
#include "Exception.h"

MINA_NAMESPACE_START

CThrowable::CThrowable(int aiState, int aiErrno, char const* apcMessage)
    : miState(aiState)
    , miErrno(aiErrno)
    , moMessage(apcMessage)
{
}

int CThrowable::GetState(void) const
{
    return this->miState;
}
    
int CThrowable::GetErrno(void) const
{
    return this->miErrno;
}
    
char const* CThrowable::GetMessage(void) const
{
    return this->moMessage.c_str();
}

MINA_NAMESPACE_END
