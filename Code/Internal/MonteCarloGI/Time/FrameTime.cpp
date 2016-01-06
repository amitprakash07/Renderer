#include "FrameTime.h"
#include <assert.h>
#include "TimeHelper.h"
#include <assert.h>

namespace myEngine
{
	namespace Timing
	{
		Clock* Clock::createAndStart()
		{
			LARGE_INTEGER *tempFrequency = new LARGE_INTEGER;
			LARGE_INTEGER *tempStartCounter = new LARGE_INTEGER;
			assert(QueryPerformanceFrequency(tempFrequency)); //takes Pointer
			assert(QueryPerformanceCounter(tempStartCounter)); //tekes Pointer
			return (new Clock(tempFrequency, tempStartCounter));
		}

		Clock::Clock(LARGE_INTEGER * i_frequency, LARGE_INTEGER * i_hRCounter)
		{
			clockFrequency = i_frequency->QuadPart;
			lastTick = i_hRCounter->QuadPart;
			currentTick = 0;
			isTimerPaused = false;
			pauseTimerTick = 0;
			deltaTime = 0;
			delete i_frequency;
			delete i_hRCounter;
			i_frequency = nullptr;
			i_hRCounter = nullptr;
		}

		void Clock::pauseTimer(bool i_input)
		{
			if (i_input == true)
			{
				isTimerPaused = true;
				LARGE_INTEGER *tempTick = new LARGE_INTEGER;
				assert(QueryPerformanceCounter(tempTick));
				pauseTimerTick = tempTick->QuadPart;
				delete tempTick;
			} // end - if
			else
				isTimerPaused = false;
		}

		void Clock::updateDeltaTime()
		{
			LARGE_INTEGER *tempTick = new LARGE_INTEGER;
			assert(QueryPerformanceCounter(tempTick)); //takes Pointer
			currentTick = tempTick->QuadPart;
			deltaTime = ((float)(myEngine::Timing::getTimeDiff(lastTick, currentTick)) / (float)clockFrequency);

			//checking timer was paused or not, if stopped subtracting the pause time from the time difference
			if (pauseTimerTick != 0)
			{
				deltaTime -= static_cast<uint32_t>((myEngine::Timing::getTimeDiff(pauseTimerTick, currentTick)) / clockFrequency);
				pauseTimerTick = 0;
			}

			//	if (timeDiff > (1 / 60)) timeDiff = (1 / 60);
			lastTick = currentTick;
			delete tempTick;
		}

		
		float Clock::getdeltaTime()
		{
			return deltaTime;
		}

		
		uint64_t Clock::getCurrentTick()
		{
			LARGE_INTEGER *tempTick = new LARGE_INTEGER;
			assert(QueryPerformanceCounter(tempTick));
			return (tempTick->QuadPart);
		}

		float Clock::getdeltaTimeDuringFrame()
		{
			LARGE_INTEGER *tempTick = new LARGE_INTEGER;
			assert(QueryPerformanceCounter(tempTick)); //takes Pointer
			uint64_t tempCurrentTick = tempTick->QuadPart;
			float o_deltaTime = ((float)(myEngine::Timing::getTimeDiff(lastTick, tempCurrentTick)) / (float)clockFrequency);
			delete tempTick;
			return o_deltaTime;
		}


		bool Clock::getTimerStatus()
		{
			return isTimerPaused;
		}
	}//namespace Timing
}//namspace myEngine