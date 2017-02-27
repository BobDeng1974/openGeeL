#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, float resolution = 1.f);
		~BlurredPostEffect();

		virtual void init(ScreenQuad& screen);

	protected:
		virtual void bindValues();

	private:
		float resolution;
		PostProcessingEffect& effect;
		PostProcessingEffect& blur;
		FrameBuffer effectBuffer;
		FrameBuffer blurBuffer;
		ScreenQuad* effectScreen = nullptr;
	};
}

#endif