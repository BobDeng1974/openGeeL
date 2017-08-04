#include "rendertime.h"
#include "application.h"
#include "threading.h"

namespace geeL{

	long Time::deltaTimeMS() const {
		return delta().count();
	}

	float Time::deltaTime() const {
		return float(deltaTimeMS()) / 1000.f;
	}

	void Time::update() {
		TimePoint currFrame = Clock::now();
		TimeFrame dur = currFrame - lastFrame();
		delta = std::chrono::duration_cast<MS>(dur);

		lastFrame = currFrame;
	}

	void Time::reset() {
		lastFrame = Clock::now();
	}

}