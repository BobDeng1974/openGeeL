#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

//Two pass gaussian blur
class GaussianBlur : public PostProcessingEffect {

public:
	GaussianBlur(unsigned int strength = 1);

	void setKernel(float newKernel[5]);
	virtual void setScreen(ScreenQuad& screen);

protected:
	virtual void bindValues();
	virtual void bindToScreen();

private:
	unsigned int currBuffer;
	unsigned int amount;
	unsigned int maxAmount = 10;
	float kernel[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
	FrameBuffer frameBuffers[2];


};

}

#endif
