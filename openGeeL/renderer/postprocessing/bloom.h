#ifndef BLOOM_H
#define BLOOM_H

#include "framebuffer/framebuffer.h"
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
		Bloom(BrightnessFilterCutoff& filter, 
			GaussianBlurBase& blur, 
			ResolutionPreset effectResolution = ResolutionPreset::FULLSCREEN, 
			ResolutionPreset blurResolution = ResolutionPreset::FULLSCREEN);

		void setScatter(float scatter);
		float getScatter() const;

		virtual std::string toString() const;

	private:
		BrightnessFilterCutoff& filter;

	};


	inline std::string Bloom::toString() const {
		return "Bloom";
	}

}

#endif
