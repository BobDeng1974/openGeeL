#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

	class DefaultPostProcess : public PostProcessingEffect {

	public:
		DefaultPostProcess(float exposure = 1.f);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);

		float getExposure() const;
		void setExposure(float exposure);


	protected:
		float exposure;
		ShaderLocation exposureLocation;

	};
}

#endif

