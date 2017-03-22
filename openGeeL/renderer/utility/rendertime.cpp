#include <glfw3.h>
#include "rendertime.h"

namespace geeL{

	float Time::deltaTime;
	float Time::lastFrame;

	void Time::update() {

		float currentFrame = (float)glfwGetTime();
		Time::deltaTime = currentFrame - Time::lastFrame;
		Time::lastFrame = currentFrame;
	}


	RenderTime::RenderTime() : lastTime(0.f) {
		passes[RenderPass::Geometry] = 0.f;
		passes[RenderPass::Shadow] = 0.f;
		passes[RenderPass::SSAO] = 0.f;
		passes[RenderPass::Lighting] = 0.f;
		passes[RenderPass::PostProcessing] = 0.f;
		passes[RenderPass::GUI] = 0.f;
	}


	void RenderTime::reset() {
		lastTime = (float)glfwGetTime();
	}

	void RenderTime::update(RenderPass pass) {
		float currTime = (float)glfwGetTime();
		float delta = currTime - lastTime;
		lastTime = currTime;

		passes[pass] = delta;
	}

	float RenderTime::getTime(RenderPass pass) const {
		return passes.at(pass);
	}
}