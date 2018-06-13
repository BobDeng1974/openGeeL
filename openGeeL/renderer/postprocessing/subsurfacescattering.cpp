#include "subsurfacescattering.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"

namespace geeL {

	SubsurfaceScattering::SubsurfaceScattering(float sigma, unsigned int kernelSize, float falloff) 
		: SeparatedGaussian("shaders/postprocessing/subsurfacescattering.frag", sigma, kernelSize, falloff) {}


	void SubsurfaceScattering::init(const PostProcessingParameter& parameter) {
		GaussianBlur::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "position");
		addTextureSampler(provider->requestProperties(), "properties");
	}

}