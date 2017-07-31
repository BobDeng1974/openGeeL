#include "cameras/camera.h"
#include "framebuffer/framebuffer.h"
#include "ssrr.h"

using namespace std;

namespace geeL {

	SSRR::SSRR(unsigned int stepCount, float stepSize, float stepSizeGain) 
		: PostProcessingEffectFS("renderer/postprocessing/ssrr.frag"), 
			steps(stepCount), stepSize(stepSize), stepGain(stepSizeGain) {}

	SSRR::SSRR(string fragmentPath, int stepCount, float stepSize, float stepSizeGain)
		: PostProcessingEffectFS(fragmentPath), steps(stepCount),
			stepSize(stepSize), stepGain(stepSizeGain) {}


	void SSRR::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<int>("stepCount", steps);
		shader.bind<float>("stepSize", stepSize);
		shader.bind<float>("stepGain", stepGain);

		shader.bind<int>("effectOnly", onlyEffect);
		projectionLocation = shader.getLocation("projection");
	}

	void SSRR::bindValues() {
		shader.bind<glm::mat4>(projectionLocation, camera->getProjectionMatrix());
	}

	void SSRR::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionRoughness");
		addImageBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
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


	MultisampledSSRR::MultisampledSSRR(unsigned int sampleCount, unsigned int stepCount, float stepSize, float stepSizeGain)
		: SSRR("renderer/postprocessing/ssrr2.frag", stepCount, stepSize, stepSizeGain), samples(sampleCount) {}


	void MultisampledSSRR::init(ScreenQuad & screen, DynamicBuffer& buffer, const Resolution& resolution) {
		SSRR::init(screen, buffer, resolution);

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
