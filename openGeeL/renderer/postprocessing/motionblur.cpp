#include <iostream>
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../transformation/transform.h"
#include "gaussianblur.h"
#include "motionblur.h"

namespace geeL {

	MotionBlur::MotionBlur(GaussianBlur& blur, float strength) 
		: PostProcessingEffect("renderer/postprocessing/motionblur.frag"),
			strength(strength), blur(blur) {}


	void MotionBlur::setBuffer(const Texture& texture) {
		PostProcessingEffect::setBuffer(texture);

		blur.setBuffer(texture);
	}

	void MotionBlur::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		samplesLocation = shader.getLocation("maxSamples");
		strengthLocation = shader.getLocation("strength");
		offsetLocation = shader.getLocation("offset");

		float resolution = 0.4f;
		screenInfo = &buffer.info;
		prevFrame.init(int(screenInfo->width * resolution), int(screenInfo->height * resolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);

		blur.init(screen, prevFrame);
		addBuffer(prevFrame.getTexture(), "prevFrame");
	}

	void MotionBlur::bindValues() {
		float diff;
		glm::vec3 offset;

		if (camera != nullptr) {
			const Transform& transform = camera->transform;
			glm::vec3 currPosition = transform.getPosition() + 20.f * transform.getForwardDirection();
			diff = glm::length(currPosition - prevPosition);

			glm::vec3 a = camera->TranslateToScreenSpace(currPosition);
			glm::vec3 b = camera->TranslateToScreenSpace(prevPosition);
			offset = a - b;

			prevPosition = currPosition;
		}
		else {
			diff = 1.f;
			offset = glm::vec3(0.1f);
			std::cout << "No camera attached to motion blur. Effect won't be completely functional.\n";
		}
		
		shader.setVector3(offsetLocation, offset);

		float value = strength * diff;
		value = (value > 1.f) ? 1.f : value;
		shader.setFloat(strengthLocation, value);
		shader.setFloat(samplesLocation, ceil(9.f * value + 1.f));
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