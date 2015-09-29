#include "Filter.h"
#include "Log.h"
#include "Session.h"
#include "Singleton.h"

MINA_NAMESPACE_START

CNextFilter::CNextFilter(const char *apcName, CFilter* apoFilter)
{
    assert(apcName != NULL);
    strncpy(this->macName, apcName, sizeof(macName));
    this->mpoFilter = apoFilter;
}
void
CNextFilter::SessionOpen(SessionType aoSession)
{
    mpoFilter->SessionOpen(aoSession, Next()->Data<CNextFilter>());
}
void
CNextFilter::SessionClose(SessionType aoSession)
{
    mpoFilter->SessionClose(aoSession, Next()->Data<CNextFilter>());
}
void
CNextFilter::SessionIdle(SessionType aoSession, uint32_t auStatus)
{
    mpoFilter->SessionIdle(aoSession, auStatus, Next()->Data<CNextFilter>());

}

void
CNextFilter::MessageRecived(SessionType aoSession, CAny& aoMessage)
{
    mpoFilter->MessageRecived(aoSession, aoMessage, Next()->Data<CNextFilter>());
}

void
CNextFilter::MessageFinished(SessionType aoSession, CAny& aoMessage)
{
    mpoFilter->MessageFinished(aoSession, aoMessage, Next()->Data<CNextFilter>());

}
void
CNextFilter::FilterClose(SessionType aoSession)
{
    mpoFilter->FilterClose(aoSession, Next()->Data<CNextFilter>());

}
void
CNextFilter::FilterWrite(SessionType aoSession, CAny& aoMessage)
{
    mpoFilter->FilterWrite(aoSession, aoMessage, Next()->Data<CNextFilter>());

}


void CFilter::SessionOpen(SessionType aoSession, CNextFilter *apoFilter)
{
    apoFilter->SessionOpen(aoSession);
}
void CFilter::SessionClose(SessionType aoSession, CNextFilter *apoFilter)
{
    apoFilter->SessionClose(aoSession);
}
void CFilter::SessionIdle(SessionType aoSession, uint32_t auStatus, CNextFilter *apoFilter)
{
    apoFilter->SessionIdle(aoSession, auStatus);
}
void CFilter::MessageRecived(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    apoFilter->MessageRecived(aoSession, aoMessage);
}
void CFilter::MessageFinished(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    apoFilter->MessageFinished(aoSession, aoMessage);
}
void CFilter::FilterClose(SessionType aoSession, CNextFilter *apoFilter)
{
    apoFilter->FilterClose(aoSession);
}
void CFilter::FilterWrite(SessionType aoSession, CAny& aoMessage, CNextFilter *apoFilter)
{
    apoFilter->FilterWrite(aoSession, aoMessage);
}




MINA_NAMESPACE_END
