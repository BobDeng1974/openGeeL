#ifndef BLOOM_H
#define BLOOM_H

#include "../framebuffer/framebuffer.h"
#include "brightnessfilter.h"
#include "blurredeffect.h"
#include "postprocessing.h"

namespace geeL {

	class ColorBuffer;
	class GaussianBlur;
	class ScreenQuad;

	
	//Post effect that introduces light bleeding into the scene
	class Bloom : public BlurredPostEffect {

	public:
		Bloom(BrightnessFilter& filter, GaussianBlur& blur, 
			float effectResolution = 1.0f, float blurResolution = 1.f);

		void setScatter(float scatter);
		float getScatter() const;

	private:
		BrightnessFilter& filter;

	};
}

#endif
