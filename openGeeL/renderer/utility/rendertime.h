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

	//Class that holds information about elapsed time inside the application
	class RenderTime {

	public:
		static float deltaTime;

		static void update();

	private:
		static float lastFrame;

	};


	class Time {

	public:
		long deltaTime() const;

		void update();
		void reset();

	private:
		AtomicWrapper<TimePoint> lastFrame;
		AtomicWrapper<MS> delta;


	};


}

#endif

