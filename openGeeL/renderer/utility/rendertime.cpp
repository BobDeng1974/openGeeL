#include <glfw3.h>

#include "rendertime.h"


namespace geeL{

	float RenderTime::delta;
	float RenderTime::lastFrame;

	float RenderTime::deltaTime() {
		return delta;
	}

	void RenderTime::update() {

		float currentFrame = (float)glfwGetTime();
		RenderTime::delta = currentFrame - RenderTime::lastFrame;
		RenderTime::lastFrame = currentFrame;
	}


	long Time::deltaTimeMS() const {
		return delta().count();
	}

	float Time::deltaTime() const {
		return float(deltaTimeMS() / 1000L);
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