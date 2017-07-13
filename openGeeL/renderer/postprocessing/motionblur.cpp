#include <iostream>
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "../transformation/transform.h"
#include "gaussianblur.h"
#include "motionblur.h"

namespace geeL {

	MotionBlur::MotionBlur(float strength, unsigned int LOD)
		: PostProcessingEffect("renderer/postprocessing/motionblur.frag"),
			strength(strength), LOD(LOD) {}

	MotionBlur::MotionBlur(const std::string& shaderPath, float strength, unsigned int LOD)
		: PostProcessingEffect(shaderPath), strength(strength), LOD(LOD) {}


	void MotionBlur::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		samplesLocation = shader.getLocation("maxSamples");
		strengthLocation = shader.getLocation("strength");
		offsetLocation = shader.getLocation("offset");

		screenInfo = &buffer.info;
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
		
		shader.setVector3(offsetLocation, offset * 2.f);

		float value = strength * diff;
		float detail = float(LOD) - 1.f;
		value = (value > 1.f) ? 1.f : value;
		shader.setFloat(strengthLocation, strength);
		shader.setFloat(samplesLocation, ceil(detail * value + 1.f));
	}

	

	float MotionBlur::getStrength() const {
		return strength;
	}

	void MotionBlur::setStrength(float value) {
		if (strength != value && value > 0.f && value < 1.f)
			strength = value;
	}

	unsigned int MotionBlur::getLevelOfDetail() const {
		return LOD;
	}

	void MotionBlur::setLevelOfDetail(unsigned int value) {
		if (LOD != value && LOD > 0 && LOD < 50) {
			LOD = value;
		}
	}


	MotionBlurPerPixel::MotionBlurPerPixel(VelocityBuffer& velocity, float strength, unsigned int LOD)
		: MotionBlur("renderer/postprocessing/motionblur2.frag", strength, LOD), 
			velocity(velocity) {}


	void MotionBlurPerPixel::init(ScreenQuad & screen, const FrameBuffer & buffer) {
		MotionBlur::init(screen, buffer);

		float resolution = 1.f;
		velocityBuffer.init(int(screenInfo->width * resolution), int(screenInfo->height * resolution),
			ColorType::RGBA16, FilterMode::Linear, WrapMode::ClampEdge);
		velocity.init(screen, velocityBuffer);

		addImageBuffer(velocityBuffer.getTexture(), "velocity");
	}

	void MotionBlurPerPixel::bindValues() {
		velocityBuffer.fill(velocity);

		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		parentBuffer->bind();

		shader.use();
		shader.setFloat(strengthLocation, getStrength());
	}



	VelocityBuffer::VelocityBuffer() 
		: PostProcessingEffect("renderer/postprocessing/velocity.frag") {}


	void VelocityBuffer::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		screenInfo = &buffer.info;

		float resolution = 1.f;
		positionBuffer.init(int(screenInfo->width * resolution), int(screenInfo->height * resolution),
			ColorType::RGBA16, FilterMode::Linear, WrapMode::ClampEdge);
		prevPositionEffect.init(screen, positionBuffer);

		addImageBuffer(positionBuffer.getTexture(), "previousPosition");
	}

	void VelocityBuffer::bindValues() {
		if (camera == nullptr)
			throw "No camera attached to velocity buffer\n";

		const Transform& transform = camera->transform;
		glm::vec3 currPosition = transform.getPosition() + 20.f * transform.getForwardDirection();
		glm::vec3 a = camera->TranslateToScreenSpace(currPosition);
		glm::vec3 b = camera->TranslateToScreenSpace(prevPosition);
		glm::vec3 offset = a - b;

		prevPosition = currPosition;

		shader.setVector3("defaultOffset", offset);
		shader.setMat4("projection", camera->getProjectionMatrix());
		shader.setVector3("origin", camera->GetOriginInViewSpace());
	}

	void VelocityBuffer::draw() {
		PostProcessingEffect::draw();

		positionBuffer.fill(prevPositionEffect);
		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		parentBuffer->bind();
	}

	void VelocityBuffer::addWorldInformation(std::map<WorldMaps, const Texture*> maps) {
		prevPositionEffect.setImageBuffer(*maps[WorldMaps::PositionRoughness]);
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "currentPosition");
	}

}