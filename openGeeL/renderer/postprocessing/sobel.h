#ifndef SOBELFILTER_H
#define SOBELFILTER_H

#include "postprocessing.h"

namespace geeL {

	//Sobel filter for edge detection in images (Grayscaled)
	class SobelFilter : public PostProcessingEffectFS {

	public:
		SobelFilter(float scale = 1.f);

		virtual void init(const PostProcessingParameter& parameter);

		float getScale() const;
		void  setScale(float value);

		virtual std::string toString() const;

	private:
		float scale;
	};


	inline std::string SobelFilter::toString() const {
		return "Sobel Filter";
	}

}

#endif
