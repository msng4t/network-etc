#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <sys/timerfd.h>
#include <strings.h>
#include <cassert>
#include "Timer.h"
#include "FdTimer.h"
#include "Log.h"


MINA_NAMESPACE_START

CTimerMgr::CTimerMgr()
: m_bInCallback(false)
{
}

CTimerMgr::~CTimerMgr()
{
}

void CTimerMgr::OnEvent(int ev)
{
	if (ev & EPOLLIN)
	{
		// 定时器到期事件
		__OnTimer();
	}
}

int CTimerMgr::Init()
{
	m_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (m_fd == -1)
	{
     return -1;
	}

	// 放入主线程
}

void CTimerMgr::AddTimeout(CIoEvent* p, int msecs)
{
	if (!m_bInCallback)
	{
		gettimeofday(&m_tvNow, NULL);
	}

	// 用当前时间加上定时值, 就是定时器的触发时间
	struct timeval tv = m_tvNow;
	if (msecs >= 1000)
	{
		// 小心溢出
		tv.tv_sec += msecs / 1000;
		msecs %= 1000;
	}

	tv.tv_usec += msecs * 1000;
	if (tv.tv_usec >= 1000000)
	{
		tv.tv_sec += tv.tv_usec / 1000000;
		tv.tv_usec %= 1000000;
	}

	bool bNeedReset = false;
	{
		TSmartLock<CLock> loGuaid(m_lstLock);

		// 按到期时间顺序插入
		std::list<TimerStruct>::iterator it = m_lstTimers.begin();
		for (; it != m_lstTimers.end(); ++it)
		{
			if (it->tmo.tv_sec > tv.tv_sec || (it->tmo.tv_sec == tv.tv_sec && it->tmo.tv_usec > tv.tv_usec))
			{
				break;
			}
		}

		if (it == m_lstTimers.begin())
		{
			// 该定时器插入到队列最前端
			if (!m_bInCallback)
			{
				bNeedReset = true;
			}
		}
		m_lstTimers.insert(it, TimerStruct(p, tv));
	}

	// 因CTimerMgr会在回调结束后自动重设定时器, 所以仅当AddTimeout不在超时回调的上下文中重设定时器
	// 且仅当该超时即为下一个即将到期的定时器时重设
	if (bNeedReset)
	{
		__ResetTimer();
	}
}

void CTimerMgr::RemoveTimeout(CIoEvent* p)
{
  TSmartLock<CLock> loGuaid(m_lstLock);

	// 按到期时间顺序插入
	for (std::list<TimerStruct>::iterator it = m_lstTimers.begin(); it != m_lstTimers.end();)
	{
		if (it->obj == p)
		{
			it = m_lstTimers.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CTimerMgr::__ResetTimer()
{
	struct timeval tvNext;
	{
      TSmartLock<CLock> loGuaid(m_lstLock);
		if (m_lstTimers.empty())
			return;

		tvNext = m_lstTimers.front().tmo;
	}

	struct itimerspec newVal;
	bzero(&newVal, sizeof(struct itimerspec));

	time_t second = tvNext.tv_sec - m_tvNow.tv_sec;
	long usecond = tvNext.tv_usec - m_tvNow.tv_usec;
	if (usecond < 0)
	{
		second--;
		usecond += 1000000;
	}

	newVal.it_value.tv_sec = second;
	newVal.it_value.tv_nsec = usecond * 1000;

	if (timerfd_settime(m_fd, 0, &newVal, NULL) == -1)
	{
		LOG_ERROR("timerfd_settime failed with error:%s", strerror(errno));
		LOG_ERROR("newVal tv_sec:%" PRIu64 ", tv_nsec:%" PRIu64 "", uint64_t(newVal.it_value.tv_sec), uint64_t(newVal.it_value.tv_nsec));
		assert(false);
	}
}

void CTimerMgr::__OnTimer()
{
	uint64_t exp;
	ssize_t s = read(m_fd, &exp, sizeof(uint64_t));
	if (s != sizeof(uint64_t))
	{
//		throw exception_errno("read");
	}

	// 检查到期的定期器，并回调
	gettimeofday(&m_tvNow, NULL);
	__CheckTimeout();
	__ResetTimer();
}

void CTimerMgr::__CheckTimeout()
{
	std::list<CIoEvent *> lstTmoObjs;

//	{
//      TSmartLock<CLock> loGuaid(m_lstLock);
//		if (m_lstTimers.empty())
//			return;
//
//		for (std::list<TimerStruct>::iterator it = m_lstTimers.begin(); it != m_lstTimers.end();)
//		{
//			if (it->tmo.tv_sec < m_tvNow.tv_sec || (it->tmo.tv_sec == m_tvNow.tv_sec && it->tmo.tv_usec <= m_tvNow.tv_usec))
//			{
//				lstTmoObjs.push_back(it->obj);
//
//				// 从队列中删除
//				it = m_lstTimers.erase(it);
//			}
//			else
//			{
//				break;
//			}
//		}
//	}
//
//	if (!lstTmoObjs.empty())
//	{
//		for (std::list<CIoEvent *>::iterator it = lstTmoObjs.begin(); it != lstTmoObjs.end(); ++it)
//		{
//			m_bInCallback = true;
//			(*it)->OnTimer();
//			m_bInCallback = false;
//		}
//	}
}
MINA_NAMESPACE_END
