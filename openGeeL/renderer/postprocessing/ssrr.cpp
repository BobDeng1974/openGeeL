#include "../cameras/camera.h"
#include "../framebuffer/framebuffer.h"
#include "ssrr.h"

using namespace std;

namespace geeL {

	SSRR::SSRR(unsigned int sampleCount, float sampleSize, float sampleSizeGain) 
		: PostProcessingEffect("renderer/postprocessing/ssrr.frag"), 
			samples(sampleCount), sampleSize(sampleSize), sampleGain(sampleSizeGain) {}


	void SSRR::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setInteger("stepCount", samples);
		shader.setFloat("stepSize", sampleSize);
		shader.setFloat("stepGain", sampleGain);

		shader.setInteger("effectOnly", onlyEffect);
		projectionLocation = shader.getLocation("projection");
	}

	void SSRR::bindValues() {
		shader.setMat4(projectionLocation, camera->getProjectionMatrix());
	}

	void SSRR::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::DiffuseRoughness], "gSpecular");
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		addBuffer(*maps[WorldMaps::NormalMetallic], "gNormalMet");
	}


	unsigned int SSRR::getSampleCount() const {
		return samples;
	}

	void SSRR::setSampleCount(unsigned int samples) {
		if (this->samples != samples && samples < 200) {
			this->samples = samples;

			shader.use();
			shader.setInteger("stepCount", samples);
		}
	}

	float SSRR::getSampleSize() const {
		return sampleSize;
	}

	void SSRR::setSampleSize(float size) {
		if (sampleSize != size && size > 0.f) {
			sampleSize = size;

			shader.use();
			shader.setFloat("stepSize", sampleSize);
		}
	}

	float SSRR::getSampleSizeGain() const {
		return sampleGain;
	}

	void SSRR::setSampleSizeGain(float gain) {
		if (sampleGain != gain && gain > 1.f) {
			sampleGain = gain;

			shader.use();
			shader.setFloat("stepGain", sampleGain);
		}
	}


}
