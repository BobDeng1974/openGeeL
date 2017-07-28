#include <glfw3.h>

#include "rendertime.h"


namespace geeL{

	float RenderTime::deltaTime;
	float RenderTime::lastFrame;

	void RenderTime::update() {

		float currentFrame = (float)glfwGetTime();
		RenderTime::deltaTime = currentFrame - RenderTime::lastFrame;
		RenderTime::lastFrame = currentFrame;
	}


	long Time::deltaTime() const {
		return delta.count();
	}

	void Time::update() {
		TimePoint currFrame = Clock::now();
		TimeFrame dur = currFrame - lastFrame;
		delta = std::chrono::duration_cast<MS>(dur);

		lastFrame = currFrame;
	}

	void Time::reset() {
		lastFrame = Clock::now();
	}

}