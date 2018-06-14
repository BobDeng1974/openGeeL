#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "brightnessfilter.h"

namespace geeL {

	BrightnessFilter::BrightnessFilter(const std::string& path) 
		: PostProcessingEffectFS(path) {}


	BrightnessFilterCutoff::BrightnessFilterCutoff(float scatter)
		: BrightnessFilter("shaders/postprocessing/bloomfilter.frag")
		, scatter(scatter) {}

	BrightnessFilterCutoff::BrightnessFilterCutoff(const std::string& shaderPath, float scatter)
		: BrightnessFilter(shaderPath)
		, scatter(scatter) {}


	void BrightnessFilterCutoff::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		scatterLocation = shader.bind<float>("scatter", scatter);
	}

	void BrightnessFilterCutoff::setScatter(float scatter) {
		if (scatter > 0.f && scatter != this->scatter) {
			this->scatter = scatter;

			shader.bind<float>(scatterLocation, scatter);
		}
	}

	float BrightnessFilterCutoff::getScatter() const {
		return scatter;
	}

	

	BrightnessFilterSmooth::BrightnessFilterSmooth(float bias, float scale)
		: BrightnessFilter("shaders/postprocessing/brightnessfilter.frag")
		, bias(bias)
		, scale(scale) {}


	void BrightnessFilterSmooth::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<float>("bias", bias);
		shader.bind<float>("scale", scale);
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

			shader.bind<float>("bias", bias);
		}
	}

	void BrightnessFilterSmooth::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.bind<float>("scale", scale);
		}
	}


	
	EmissiveBrightnessFilter::EmissiveBrightnessFilter(float scatter)
		: BrightnessFilterCutoff("shaders/postprocessing/emissivefilter.frag", scatter) {}


	void EmissiveBrightnessFilter::init(const PostProcessingParameter& parameter) {
		BrightnessFilterCutoff::init(parameter);

		assert(provider != nullptr);
		RenderTexture* const emission = provider->requestEmission();
		if (emission) addTextureSampler(*emission, "emission");
	}


}