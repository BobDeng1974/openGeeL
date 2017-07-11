#ifndef BRIGHTNESSFILTER_H
#define BRIGHTNESSFILTER_H

#include "postprocessing.h"

namespace geeL {

	//Filter effect that filters bright colors / lights in image depending on given scatter
	class BrightnessFilter : public PostProcessingEffect {

	public:
		float scatter;

		BrightnessFilter(float scatter = 0.6f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		void setScatter(float scatter);
		float getScatter() const;

	private:
		ShaderLocation scatterLocation;

	};

}

#endif

