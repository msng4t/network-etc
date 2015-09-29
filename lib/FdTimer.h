#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>
#include <list>
#include "Define.h"
#include "Poll.h"
#include "Synch.h"

MINA_NAMESPACE_START

class CTimerMgr
{
  public:
    CTimerMgr();
    ~CTimerMgr();

  public:
    virtual void OnEvent(int ev);

    int Init();
    void AddTimeout(CIoEvent* p, int msecs);
    void RemoveTimeout(CIoEvent* p);

  private:
    void __ResetTimer();
    void __OnTimer();
    void __CheckTimeout();

  private:
    bool m_bInCallback;
    struct timeval m_tvNow;

    struct TimerStruct
    {
      public:
        TimerStruct(CIoEvent* p, const struct timeval& tv)
          : obj(p), tmo(tv)
        {
        }

      public:
        CIoEvent* obj;
        struct timeval tmo;
    };

    int m_fd;
    CLock m_lstLock;
    std::list<TimerStruct> m_lstTimers;
};

MINA_NAMESPACE_END
#endif // TIMER_H_
