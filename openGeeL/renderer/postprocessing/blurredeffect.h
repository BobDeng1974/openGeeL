#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, 
			float effectResolution = 1.f, float blurResolution = 1.f);

		virtual void setBuffer(const FrameBuffer& buffer);
		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		void resizeEffectResolution(float effectResolution);

		float getEffectResolution() const;
		float getBlurResolution() const;


	protected:
		virtual void bindValues();

	private:
		float effectResolution, blurResolution;
		PostProcessingEffect& effect;
		PostProcessingEffect& blur;
		FrameBuffer effectBuffer;
		FrameBuffer blurBuffer;
		const FrameBufferInformation* screenInfo;

	};
}

#endif