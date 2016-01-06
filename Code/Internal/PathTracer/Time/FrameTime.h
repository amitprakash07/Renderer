#ifndef __FRAME_TIME_H
#define __FRAME_TIME_H

#include <stdint.h>
#include <windows.h>


namespace myEngine
{
	namespace Timing
	{
		class Clock
		{
		public:

			static Clock*						createAndStart();
			void								pauseTimer(bool);
			bool								getTimerStatus();
			float								getdeltaTime();
			float								getdeltaTimeDuringFrame();
			void								updateDeltaTime();
			static uint64_t						getCurrentTick();

		private:

			uint64_t							lastTick;
			uint64_t							currentTick;
			uint64_t							pauseTimerTick;
			float								deltaTime;
			bool								isTimerPaused;
			uint64_t							clockFrequency;
			Clock(LARGE_INTEGER *, LARGE_INTEGER *);
		};//Clock
	}//namespace Timing
}//namespace myEngine

#endif