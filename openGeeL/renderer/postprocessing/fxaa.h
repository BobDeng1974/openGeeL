#ifndef FXAA_H
#define FXAA_H

#include "postprocessing.h"

namespace geeL {

	//Fast Approximate Anti-Aliasing post effect
	class FXAA : public PostProcessingEffect {

	public:
		FXAA(float blurMin = 0.05f, float fxaaMin = 1.f / 128.f, float fxaaClamp = 8.f);


		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		float getBlurMin() const;
		float getFXAAMin() const;
		float getFXAAClamp() const;

		void setBlurMin(float value);
		void setFXAAMin(float value);
		void setFXAAClamp(float value);


	private:
		float blurMin, fxaaMin, fxaaClamp;



	};
}

#endif

