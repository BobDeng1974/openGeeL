#ifndef RENDERTIME_H
#define RENDERTIME_H

#include <chrono>
#include <map>
#include "objectwrapper.h"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::steady_clock::time_point TimePoint;
typedef std::chrono::milliseconds MS;
typedef std::chrono::duration<float> TimeFrame;

namespace geeL {

	class Application;


	//Class that holds information about elapsed time periods
	class Time {

	public:
		//Return delta time in milliseconds
		long deltaTimeMS() const;

		//Return delta time percentage wise
		float deltaTime() const;

		void update();
		void reset();

	private:
		AtomicWrapper<TimePoint> lastFrame;
		AtomicWrapper<MS> delta;

	};


	//Class that holds time information from current thread
	class RenderTime {

	public:
		static float deltaTime();

	};


}

#endif

