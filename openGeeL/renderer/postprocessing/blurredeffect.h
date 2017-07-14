#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, 
			Resolution effectResolution = FULLSCREEN, Resolution blurResolution = FULLSCREEN);

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void draw();

		void resizeEffectResolution(float effectResolution);

		const Resolution& getEffectResolution() const;
		const Resolution& getBlurResolution() const;

	protected:
		const FrameBufferInformation* screenInfo;

		virtual void bindValues();

	private:
		Resolution effectResolution, blurResolution;
		PostProcessingEffect& effect;
		PostProcessingEffect& blur;
		ColorBuffer effectBuffer;
		ColorBuffer blurBuffer;

	};
}

#endif