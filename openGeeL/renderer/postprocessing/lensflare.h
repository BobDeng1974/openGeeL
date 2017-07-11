#ifndef LENSFLARE_H
#define LENSFLARE_H

#include "blurredeffect.h"
#include "brightnessfilter.h"

namespace geeL {

	class GaussianBlurBase;


	class LensFlareFilter : public PostProcessingEffect {

	public:
		LensFlareFilter(BrightnessFilter& filter, float resolution = 1.f, float scale = 0.5f, float samples = 4.f);

		virtual void setBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		float getScale() const;
		float getMaxSamples() const;

		void setScale(float value);
		void setMaxSamples(float value);
		void setResolution(float value);

	protected:
		virtual void bindValues();

	private:
		float scale, samples, resolution;
		BrightnessFilter& filter;
		ColorBuffer filterBuffer;

		const FrameBufferInformation* screenInfo;

	};


	class LensFlare : public BlurredPostEffect {

	public:
		LensFlareFilter& filter;
		
		LensFlare(LensFlareFilter& filter, GaussianBlurBase& blur, 
			float effectResolution = 1.f, float blurResolution = 1.f);

	};

}

#endif
