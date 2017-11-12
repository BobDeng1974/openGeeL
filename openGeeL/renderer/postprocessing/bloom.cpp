#define GLEW_STATIC
#include <glew.h>
#include "gaussianblur.h"
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "bloom.h"

namespace geeL {

	Bloom::Bloom(BrightnessFilterCutoff& filter, 
		GaussianBlurBase& blur, 
		ResolutionPreset filterResolution, 
		ResolutionPreset blurResolution)
			: BlurredPostEffect(filter, blur, filterResolution, blurResolution)
			, filter(filter) {}

	void Bloom::setScatter(float scatter) {
		filter.setScatter(scatter);
	}

	float Bloom::getScatter() const {
		return filter.getScatter();
	}


}