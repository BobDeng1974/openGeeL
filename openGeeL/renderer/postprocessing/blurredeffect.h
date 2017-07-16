#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, PostProcessingEffect& blur, 
			ResolutionScale effectResolution = FULLSCREEN, ResolutionScale blurResolution = FULLSCREEN);

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer);

		virtual void draw();

		void resizeEffectResolution(ResolutionScale effectResolution);

		const ResolutionScale& getEffectResolution() const;
		const ResolutionScale& getBlurResolution() const;

	protected:
		virtual void bindValues();

	private:
		ResolutionScale effectResolution, blurResolution;
		PostProcessingEffect& effect;
		PostProcessingEffect& blur;
		ColorBuffer effectBuffer;
		ColorBuffer blurBuffer;

	};
}

#endif