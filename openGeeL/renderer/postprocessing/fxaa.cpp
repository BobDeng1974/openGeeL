#include "fxaa.h"

namespace geeL {

	FXAA::FXAA(float blurMin, float fxaaMin, float fxaaClamp)
		: PostProcessingEffect("renderer/postprocessing/fxaa.frag"), 
			blurMin(blurMin), fxaaMin(fxaaMin), fxaaClamp(fxaaClamp) {}


	void FXAA::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("BLUR_MIN", blurMin);
		shader.setFloat("FXAA_MIN", fxaaMin);
		shader.setFloat("FXAA_CLAMP", fxaaClamp);
	}

	float FXAA::getBlurMin() const {
		return blurMin;
	}

	float FXAA::getFXAAMin() const {
		return fxaaMin;
	}

	float FXAA::getFXAAClamp() const {
		return fxaaClamp;
	}

	void FXAA::setBlurMin(float value) {
		if (blurMin != value && value >= 0.f && value <= 1.f) {
			blurMin = value;

			shader.use();
			shader.setFloat("BLUR_MIN", blurMin);
		}
	}

	void FXAA::setFXAAMin(float value) {
		if (fxaaMin != value && value >= 0.f && value <= 1.f) {
			fxaaMin = value;

			shader.use();
			shader.setFloat("FXAA_MIN", fxaaMin);
		}
	}

	void FXAA::setFXAAClamp(float value) {
		if (fxaaClamp != value && value >= 0.f && value <= 10.f) {
			fxaaClamp = value;

			shader.use();
			shader.setFloat("FXAA_CLAMP", fxaaClamp);
		}
	}


}