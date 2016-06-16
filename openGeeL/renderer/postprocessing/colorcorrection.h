#ifndef COLORCORRECTION_H
#define COLORCORRECTION_H

#include "postprocessing.h"

namespace geeL {

class ColorCorrection : public PostProcessingEffect {

public:
	float r, g , b, h, s, v;

	ColorCorrection(float red = 1.f, float green = 1.f, float blue = 1.f, 
		float hue = 1.f, float saturation = 1.f, float brightness = 1.f);


	virtual void draw();

};


}

#endif