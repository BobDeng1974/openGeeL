#include "sobel.h"

namespace geeL {

	SobelFilter::SobelFilter(float scale) 
		: PostProcessingEffectFS("shaders/postprocessing/sobel.frag")
		, scale(scale) {}


	void SobelFilter::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<float>("scale", scale);
	}

	float SobelFilter::getScale() const {
		return scale;
	}

	void SobelFilter::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.bind<float>("scale", scale);
		}
	}

}