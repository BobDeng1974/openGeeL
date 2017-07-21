#include "brightnessfilter.h"

namespace geeL {

	BrightnessFilter::BrightnessFilter(const std::string& path) : PostProcessingEffectFS(path) {}


	BrightnessFilterCutoff::BrightnessFilterCutoff(float scatter)
		: BrightnessFilter("renderer/postprocessing/bloomfilter.frag"), scatter(scatter) {}


	void BrightnessFilterCutoff::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		scatterLocation = shader.set<float>("scatter", scatter);
	}

	void BrightnessFilterCutoff::setScatter(float scatter) {
		if (scatter > 0.f && scatter != this->scatter) {
			this->scatter = scatter;

			shader.use();
			shader.set<float>(scatterLocation, scatter);
		}
	}

	float BrightnessFilterCutoff::getScatter() const {
		return scatter;
	}

	BrightnessFilterSmooth::BrightnessFilterSmooth(float bias, float scale)
		: BrightnessFilter("renderer/postprocessing/brightnessfilter.frag"), bias(bias), scale(scale) {}


	void BrightnessFilterSmooth::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.set<float>("bias", bias);
		shader.set<float>("scale", scale);
	}

	float BrightnessFilterSmooth::getBias() const {
		return bias;
	}

	float BrightnessFilterSmooth::getScale() const {
		return scale;
	}

	void BrightnessFilterSmooth::setBias(float value) {
		if (bias != value && value > 0.f) {
			bias = value;

			shader.use();
			shader.set<float>("bias", bias);
		}
	}

	void BrightnessFilterSmooth::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.use();
			shader.set<float>("scale", scale);
		}
	}

	

}