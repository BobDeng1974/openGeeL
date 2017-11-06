#ifndef FXAA_H
#define FXAA_H

#include "postprocessing.h"

namespace geeL {

	//Fast Approximate Anti-Aliasing post effect
	class FXAA : public PostProcessingEffectFS {

	public:
		FXAA(float minColorDiff = 0.01f, float fxaaMul = 0.125f, float fxaaMin = 0.00001f, float fxaaClamp = 8.f);

		virtual void init(const PostProcessingParameter& parameter);

		float getBlurMin() const;
		float getFXAAMul() const;
		float getFXAAMin() const;
		float getFXAAClamp() const;

		void setBlurMin(float value);
		void setFXAAMul(float value);
		void setFXAAMin(float value);
		void setFXAAClamp(float value);

		virtual std::string toString() const;

	private:
		float blurMin, fxaaMul, fxaaMin, fxaaClamp;

	};


	inline std::string FXAA::toString() const {
		return "FXAA";
	}

}

#endif

