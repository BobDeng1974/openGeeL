#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include "postprocessing.h"

namespace geeL {

	//Sobel filter for edge detection in images (Grayscaled)
	class SobelFilter : public PostProcessingEffect {

	public:
		SobelFilter(float scale = 1.f);

		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

		float getScale() const;
		void  setScale(float value);

	private:
		float scale;
	};
}

#endif
