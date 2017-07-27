#include "fxaa.h"
#include <iostream>

namespace geeL {

	FXAA::FXAA(float minColorDiff, float fxaaMul, float fxaaMin, float fxaaClamp)
		: PostProcessingEffectFS("renderer/postprocessing/fxaa.frag"), 
			blurMin(minColorDiff), fxaaMul(fxaaMul), fxaaMin(fxaaMin), fxaaClamp(fxaaClamp) {
	
		std::cout << "ayy\n";
	}


	void FXAA::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);

		shader.bind<float>("DIFF_THRESHOLD", blurMin);
		shader.bind<float>("FXAA_REDUCE_MUL", fxaaMul);
		shader.bind<float>("FXAA_MIN", fxaaMin);
		shader.bind<float>("FXAA_CLAMP", fxaaClamp);
	}

	float FXAA::getBlurMin() const {
		return blurMin;
	}

	float FXAA::getFXAAMul() const {
		return fxaaMul;
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

			shader.bind<float>("DIFF_THRESHOLD", blurMin);
		}
	}

	void FXAA::setFXAAMul(float value) {
		if (fxaaMul != value && value >= 0.f && value <= 1.f) {
			fxaaMul = value;

			shader.bind<float>("FXAA_REDUCE_MUL", fxaaMul);
		}
	}

	void FXAA::setFXAAMin(float value) {
		if (fxaaMin != value && value >= 0.f) {
			fxaaMin = value;

			shader.bind<float>("FXAA_MIN", fxaaMin);
		}
	}

	void FXAA::setFXAAClamp(float value) {
		if (fxaaClamp != value && value >= 0.f) {
			fxaaClamp = value;

			shader.bind<float>("FXAA_CLAMP", fxaaClamp);
		}
	}


}