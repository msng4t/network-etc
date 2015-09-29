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

		// �õ���ǰע����¼�����
		uint32_t GetCurrentSel() const
		{
			return m_uCurrentSel;
		}

		void SetCurrentSel(uint32_t uSel)
		{
			m_uCurrentSel = uSel;
		}

		// ��������Ƴ�
		void MarkRemoval()
		{
			m_pSelector->MarkRemoval(this);
		}

	public:
		// ���ļ��������ϲ������¼�, ָ���Ļص�����
		virtual void OnEvent(int ev) = 0;

	protected:
		uint32_t m_uCurrentSel;

		// ÿ���¼�Դ, ����ӳ�䵽һ���ļ�������, ����epoll
		int m_fd;
		friend class CEpollSelector;

	};
}

#endif // EVENT_SOURCE_H_
