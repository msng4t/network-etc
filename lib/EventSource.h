#ifndef EVENT_SOURCE_H_
#define EVENT_SOURCE_H_

#include "Log.h"

namespace sox
{
	class IEventSource
	{
	public:
		IEventSource() : m_uCurrentSel(0), m_fd(-1), m_pSelector(0)
		{
		}

		IEventSource(int fd) : m_uCurrentSel(0), m_fd(fd), m_pSelector(0)
		{
		}

		virtual ~IEventSource()
		{
			if (m_fd != -1)
			{
				if (m_pSelector)
				{
					m_pSelector->Remove(this);
				}

				::close(m_fd);
			}
		}

		// 得到当前注册的事件类型
		uint32_t GetCurrentSel() const
		{
			return m_uCurrentSel;
		}

		void SetCurrentSel(uint32_t uSel)
		{
			m_uCurrentSel = uSel;
		}

		// 标记自身被移除
		void MarkRemoval()
		{
			m_pSelector->MarkRemoval(this);
		}

	public:
		// 在文件描述符上产生了事件, 指定的回调函数
		virtual void OnEvent(int ev) = 0;

	protected:
		uint32_t m_uCurrentSel;

		// 每个事件源, 必须映射到一个文件描述符, 用于epoll
		int m_fd;
		friend class CEpollSelector;

	};
}

#endif // EVENT_SOURCE_H_
