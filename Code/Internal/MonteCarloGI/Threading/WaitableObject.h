#ifndef __WAITABLE_OBJECT_H
#define __WAITABLE_OBJECT_H

#include <Windows.h>

namespace myEngine
{
	class WaitableObject
	{
	public:
		WaitableObject() :
			m_Handle(INVALID_HANDLE_VALUE)
		{
		}
		HANDLE GetHandle()
		{
			return m_Handle; 
		}

	protected:
		HANDLE					m_Handle;

	};

} // namespace Engine

#endif // __WAITABLE_OBJECT_H