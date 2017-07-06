#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../transformation/transform.h"
#include "gaussianblur.h"
#include "motionblur.h"
#include <iostream>

namespace geeL {

	MotionBlur::MotionBlur(GaussianBlur& blur, float strength) : PostProcessingEffect("renderer/postprocessing/motionblur.frag"),
		strength(strength), blur(blur) {}


	void MotionBlur::setBuffer(const Texture& texture) {
		PostProcessingEffect::setBuffer(texture);

		blur.setBuffer(texture);
	}

	void MotionBlur::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		strengthLocation = shader.getLocation("strength");

		float resolution = 0.4f;
		screenInfo = &buffer.info;
		prevFrame.init(int(screenInfo->width * resolution), int(screenInfo->height * resolution),
			ColorType::RGB16, FilterMode::Linear);

		blur.init(screen, prevFrame);
		addBuffer(prevFrame.getTexture(), "prevFrame");
	}

	void MotionBlur::bindValues() {
		const Transform& transform = camera->transform;
		glm::vec3 currPosition = transform.getPosition() + 20.f * transform.getForwardDirection();
		float diff = glm::length(currPosition - prevPosition);
		prevPosition = currPosition;

		float value = strength * diff;
		value = (value > 1.f) ? 1.f : value;
		shader.setFloat(strengthLocation, value);
	}

	void MotionBlur::draw() {
		PostProcessingEffect::draw();

		prevFrame.fill(blur);
		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		parentBuffer->bind();
	}

	float MotionBlur::getStrength() const {
		return strength;
	}

	void MotionBlur::setStrength(float value) {
		if (strength != value && value > 0.f && value < 1.f)
			strength = value;
	}

}