#include <iostream>
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "transformation/transform.h"
#include "gaussianblur.h"
#include "motionblur.h"

namespace geeL {

	MotionBlur::MotionBlur(float strength, unsigned int LOD)
		: PostProcessingEffectFS("renderer/postprocessing/motionblur.frag")
		, strength(strength)
		, LOD(LOD) {}

	MotionBlur::MotionBlur(const std::string& shaderPath, float strength, unsigned int LOD)
		: PostProcessingEffectFS(shaderPath), strength(strength), LOD(LOD) {}


	void MotionBlur::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		samplesLocation = shader.getLocation("maxSamples");
		strengthLocation = shader.getLocation("strength");
		offsetLocation = shader.getLocation("offset");
	}

	void MotionBlur::bindValues() {
		float diff;
		glm::vec3 offset;

		if (camera != nullptr) {
			Transform& transform = camera->transform;
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
		
		shader.bind<glm::vec3>(offsetLocation, offset * 2.f);

		float value = strength * diff;
		float detail = float(LOD) - 1.f;
		value = (value > 1.f) ? 1.f : value;
		shader.bind<float>(strengthLocation, strength);
		shader.bind<float>(samplesLocation, ceil(detail * value + 1.f));
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
		: MotionBlur("renderer/postprocessing/motionblur2.frag", strength, LOD)
		, velocity(velocity) {}

	MotionBlurPerPixel::~MotionBlurPerPixel() {
		if (velocityTexture != nullptr) delete velocityTexture;
	}


	void MotionBlurPerPixel::init(const PostProcessingParameter& parameter) {
		MotionBlur::init(parameter);

		float res = 1.f;
		Resolution velocityRes = Resolution(parentBuffer->getResolution(), 1.f);
		if (velocityTexture == nullptr)
			velocityTexture = new RenderTexture(velocityRes,
				ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::Linear);
		else
			velocityTexture->resize(velocityRes);

		velocity.init(PostProcessingParameter(parameter, velocityRes));

		addTextureSampler(*velocityTexture, "velocity");
	}

	void MotionBlurPerPixel::bindValues() {
		shader.bind<float>(strengthLocation, getStrength());
	}

	void MotionBlurPerPixel::bindSubImages() {
		parentBuffer->add(*velocityTexture);
		parentBuffer->fill(velocity, clearColor);
	}



	VelocityBuffer::VelocityBuffer() 
		: PostProcessingEffectFS("renderer/postprocessing/velocity.frag") {}

	VelocityBuffer::~VelocityBuffer() {
		if (positionTexture != nullptr) delete positionTexture;
	}


	void VelocityBuffer::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		assert(provider != nullptr);
		prevPositionEffect.setImage(provider->requestPositionRoughness());
		addTextureSampler(provider->requestPositionRoughness(), "currentPosition");


		Resolution positionRes = Resolution(parentBuffer->getResolution(), 1.f);
		if (positionTexture == nullptr)
			positionTexture = new RenderTexture(positionRes, ColorType::RGBA16,
				WrapMode::ClampEdge, FilterMode::Linear);
		else
			positionTexture->resize(positionRes);

		prevPositionEffect.init(PostProcessingParameter(parameter, positionRes));

		addTextureSampler(*positionTexture, "previousPosition");
	}

	void VelocityBuffer::bindValues() {
		if (camera == nullptr)
			throw "No camera attached to velocity buffer\n";

		Transform& transform = camera->transform;
		glm::vec3 currPosition = transform.getPosition() + 20.f * transform.getForwardDirection();
		glm::vec3 a = camera->TranslateToScreenSpace(currPosition);
		glm::vec3 b = camera->TranslateToScreenSpace(prevPosition);
		glm::vec3 offset = a - b;

		prevPosition = currPosition;

		shader.bind<glm::vec3>("defaultOffset", offset);
		shader.bind<glm::vec3>("origin", camera->GetOriginInViewSpace());

		camera->bindProjectionMatrix(shader, "projection");
	}

	void VelocityBuffer::draw() {
		PostProcessingEffectFS::draw();

		parentBuffer->add(*positionTexture);
		parentBuffer->fill(prevPositionEffect, clearColor);

	}

}