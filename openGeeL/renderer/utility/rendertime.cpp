#include "rendertime.h"

namespace geeL{

	float Time::deltaTime;
	float Time::lastFrame;

	void Time::update() {

		float currentFrame = glfwGetTime();
		Time::deltaTime = currentFrame - Time::lastFrame;
		Time::lastFrame = currentFrame;
	}
}