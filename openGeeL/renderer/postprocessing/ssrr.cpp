#include "cameras/camera.h"
#include "framebuffer/framebuffer.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "ssrr.h"

using namespace std;

namespace geeL {

	SSRR::SSRR(unsigned int stepCount, float stepSize, float stepSizeGain) 
		: PostProcessingEffectFS("shaders/postprocessing/ssrr.frag")
		, steps(stepCount)
		, stepSize(stepSize)
		, stepGain(stepSizeGain) {}

	SSRR::SSRR(string fragmentPath, int stepCount, float stepSize, float stepSizeGain)
		: PostProcessingEffectFS(fragmentPath), steps(stepCount)
		, stepSize(stepSize)
		, stepGain(stepSizeGain) {}


	void SSRR::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "gPositionRoughness");
		addTextureSampler(provider->requestNormal(), "gNormalMet");
		addTextureSampler(provider->requesOcclusionEmissivityRoughnessMetallic(), "gProperties");

		shader.bind<int>("stepCount", steps);
		shader.bind<float>("stepSize", stepSize);
		shader.bind<float>("stepGain", stepGain);

		shader.bind<int>("effectOnly", onlyEffect);
		projectionLocation = shader.getLocation("projection");
	}

	void SSRR::bindValues() {
		camera->bindProjectionMatrix(shader, projectionLocation);
	}

	unsigned int SSRR::getStepCount() const {
		return steps;
	}

	void SSRR::setStepCount(unsigned int steps) {
		if (this->steps != steps && steps < 200) {
			this->steps = steps;

			shader.bind<int>("stepCount", steps);
		}
	}

	float SSRR::getStepSize() const {
		return stepSize;
	}

	void SSRR::setStepSize(float size) {
		if (stepSize != size && size > 0.f) {
			stepSize = size;

			shader.bind<float>("stepSize", stepSize);
		}
	}

	float SSRR::getStepSizeGain() const {
		return stepGain;
	}

	void SSRR::setStepSizeGain(float gain) {
		if (stepGain != gain && gain > 1.f) {
			stepGain = gain;

			shader.bind<float>("stepGain", stepGain);
		}
	}


	MultisampledSSRR::MultisampledSSRR(unsigned int sampleCount, 
		unsigned int stepCount, 
		float stepSize, 
		float stepSizeGain)
			: SSRR("shaders/postprocessing/ssrr2.frag", stepCount, stepSize, stepSizeGain)
			, samples(sampleCount) {}


	void MultisampledSSRR::init(const PostProcessingParameter& parameter) {
		SSRR::init(parameter);

		shader.bind<int>("sampleCount", samples);
	}

	unsigned int MultisampledSSRR::getSampleCount() const {
		return samples;
	}

	void MultisampledSSRR::setSampleCount(unsigned int samples) {
		if (this->samples != samples && samples < 100) {
			this->samples = samples;

			shader.bind<int>("sampleCount", samples);
		}
	}


}
