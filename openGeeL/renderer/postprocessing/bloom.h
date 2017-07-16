#ifndef BLOOM_H
#define BLOOM_H

#include "../framebuffer/framebuffer.h"
#include "brightnessfilter.h"
#include "blurredeffect.h"
#include "postprocessing.h"

namespace geeL {

	class ColorBuffer;
	class GaussianBlurBase;
	class ScreenQuad;

	
	//Post effect that introduces light bleeding into the scene
	class Bloom : public BlurredPostEffect {

	public:
		Bloom(BrightnessFilterCutoff& filter, GaussianBlurBase& blur, 
			ResolutionScale effectResolution = FULLSCREEN, ResolutionScale blurResolution = FULLSCREEN);

		void setScatter(float scatter);
		float getScatter() const;

	private:
		BrightnessFilterCutoff& filter;

	};
}

#endif
