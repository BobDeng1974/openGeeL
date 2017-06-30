#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, 
			float effectResolution = 1.f, float blurResolution = 1.f);

		virtual void setBuffer(const Texture& texture);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void draw();

		void resizeEffectResolution(float effectResolution);

		float getEffectResolution() const;
		float getBlurResolution() const;


	protected:
		virtual void bindValues();

	private:
		float effectResolution, blurResolution;
		PostProcessingEffect& effect;
		PostProcessingEffect& blur;
		ColorBuffer effectBuffer;
		ColorBuffer blurBuffer;
		const FrameBufferInformation* screenInfo;

	};
}

#endif