#include <iostream>
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "shader/shaderreader.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "transformation/transform.h"
#include "gaussianblur.h"
#include "motionblur.h"

namespace geeL {

	MotionBlur::MotionBlur(float strength, float sigma, unsigned int LOD, unsigned int maxLOD)
		: MotionBlur("shaders/postprocessing/motionblur.frag", strength, sigma, LOD, maxLOD) {}

	MotionBlur::MotionBlur(const std::string& shaderPath, 
		float strength, float sigma, 
		unsigned int LOD, unsigned int maxLOD)
			: PostProcessingEffectFS(defaultVertexPath, shaderPath,
				StringReplacement("^const unsigned int maxSamples =\\s+([0-9]+){1};\\s?",
					std::to_string(maxLOD), 1))
			, strength(0.f)
			, sigma(0.f)
			, LOD(0)
			, maxLOD(maxLOD) {
	
		samplesLocation = shader.getLocation("sampleSize");
		strengthLocation = shader.getLocation("strength");

		setStrength(strength);
		setSigma(sigma);
		setLevelOfDetail(LOD);
	}


	void MotionBlur::bindValues() {
		assert(camera != nullptr);

		Transform& transform = camera->transform;
		glm::vec3 currPosition = transform.getPosition() + 5.f * transform.getForwardDirection();


		while (positions.size() >= LOD)
			positions.pop_back();

		positions.push_front(currPosition);


		size_t i = 0;
		glm::vec3 offset(0.f);
		for (auto it(positions.begin()); it != prev(positions.end()); it++) {

			glm::vec3 a = *it;
			glm::vec3 b = *next(it);

			glm::vec3 off = camera->TranslateToScreenSpace(a) - camera->TranslateToScreenSpace(b);

			offset += off * strength;
			shader.bind<glm::vec3>("offsets[" + std::to_string(i++) + "]", offset);
		}
	}
	

	float MotionBlur::getStrength() const {
		return strength;
	}

	float MotionBlur::getSigma() const {
		return sigma;
	}

	unsigned int MotionBlur::getLevelOfDetail() const {
		return LOD;
	}


	void MotionBlur::setStrength(float value) {
		if (strength != value && value > 0.f && value < 1.f)
			strength = value;
	}

	void MotionBlur::setSigma(float value) {
		if (sigma != value && value > 0.f) {
			sigma = value;

			updateKernel();
		}
	}

	void MotionBlur::setLevelOfDetail(unsigned int value) {
		if (LOD != value && value > 2 && value <= maxLOD) {
			LOD = value;

			shader.bind<float>(samplesLocation, LOD - 1);
			updateKernel();
		}
	}

	void MotionBlur::updateKernel() {
		if (LOD == 0 || sigma == 0.f) return;

		std::vector<float> kernel = std::move(GaussianBlur::computeKernel(sigma, LOD - 1, true));
		for (int i = 0; i < kernel.size(); i++)
			shader.bind<float>("kernel[" + std::to_string(i) + "]", kernel[i]);

	}



	MotionBlurPerPixel::MotionBlurPerPixel(VelocityBuffer& velocity, float strength, unsigned int LOD)
		: MotionBlur("shaders/postprocessing/motionblur2.frag", strength, LOD)
		, velocity(velocity) {}

	MotionBlurPerPixel::~MotionBlurPerPixel() {
		if (velocityTexture != nullptr) delete velocityTexture;
	}


	void MotionBlurPerPixel::init(const PostProcessingParameter& parameter) {
		MotionBlur::init(parameter);

		float res = 1.f;
		Resolution velocityRes = Resolution(parameter.resolution, 1.f);
		if (velocityTexture == nullptr)
			velocityTexture = &provider->requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGB16,
				FilterMode::Linear, WrapMode::ClampEdge);
		else
			velocityTexture->resize(velocityRes);

		velocity.init(PostProcessingParameter(parameter, velocityRes));

		addTextureSampler(*velocityTexture, "velocity");
	}

	void MotionBlurPerPixel::bindValues() {
		velocity.bindValues();

		shader.bind<float>(strengthLocation, getStrength());
	}

	void MotionBlurPerPixel::drawSubImages() {
		parentBuffer->add(*velocityTexture);
		parentBuffer->fill(velocity, clearColor);
	}



	VelocityBuffer::VelocityBuffer() 
		: PostProcessingEffectFS("shaders/postprocessing/velocity.frag") {}

	VelocityBuffer::~VelocityBuffer() {
		if (positionTexture != nullptr) delete positionTexture;
	}


	void VelocityBuffer::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		assert(provider != nullptr);
		prevPositionEffect.setImage(provider->requestPosition());
		addTextureSampler(provider->requestPosition(), "currentPosition");


		Resolution positionRes = Resolution(parameter.resolution, 1.f);
		if (positionTexture == nullptr)
			positionTexture = &provider->requestTextureManual(ResolutionPreset::FULLSCREEN, ColorType::RGB16,
				FilterMode::Linear, WrapMode::ClampEdge);
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