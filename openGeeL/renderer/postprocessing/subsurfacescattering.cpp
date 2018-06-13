#include "subsurfacescattering.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"

namespace geeL {

	SubsurfaceScattering::SubsurfaceScattering(float sigma, unsigned int kernelSize, float falloff) 
		: SeparatedGaussian("shaders/postprocessing/subsurfacescattering.frag", sigma, kernelSize) {
	
		setFalloff(falloff);
	}


	void SubsurfaceScattering::init(const PostProcessingParameter& parameter) {
		GaussianBlur::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "position");
		addTextureSampler(provider->requestProperties(), "properties");
	}


	float SubsurfaceScattering::getFalloff() const {
		return falloff;
	}

	void SubsurfaceScattering::setFalloff(float value) {
		if (falloff != value && value > 0.f) {
			falloff = value;

			shader.bind<float>("falloff", value);
		}
	}

}