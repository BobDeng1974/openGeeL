#include "sobel.h"

namespace geeL {

	SobelFilter::SobelFilter(float scale) 
		: PostProcessingEffectFS("renderer/postprocessing/sobel.frag"), scale(scale) {}


	void SobelFilter::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<float>("scale", scale);
	}

	float SobelFilter::getScale() const {
		return scale;
	}

	void SobelFilter::setScale(float value) {
		if (scale != value && value > 0.f) {
			scale = value;

			shader.use();
			shader.bind<float>("scale", scale);
		}
	}

}