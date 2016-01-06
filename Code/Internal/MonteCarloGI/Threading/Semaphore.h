#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#include "WaitableObject.h"

namespace myEngine
{
	class Semaphore : public WaitableObject
	{
	public:
		Semaphore(const char * i_pName, unsigned int i_CountInit, unsigned int  i_CountMax);
		~Semaphore();

		void Increment(unsigned int i_count = 1);
		void WaitAndDecrement(void);
	};

} // namespace Engine

#endif // __SEMAPHORE_H