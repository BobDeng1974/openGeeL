#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include "postprocessing.h"

namespace geeL {

	//Sobel filter for edge detection in images (Grayscaled)
	class SobelFilter : public PostProcessingEffect {

	public:
		SobelFilter(float scale = 1.f);

	protected:
		virtual void bindValues();

	private:
		float scale;
	};
}

#endif
