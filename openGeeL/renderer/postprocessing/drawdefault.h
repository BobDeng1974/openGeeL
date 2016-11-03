#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "../texturing/simpletexture.h"
#include "postprocessing.h"

namespace geeL {

class DefaultPostProcess : public PostProcessingEffect {

public:
	float exposure;

	DefaultPostProcess(float exposure = 1.f);

	virtual void setScreen(ScreenQuad& screen);

protected:
	virtual void bindValues();

private:
	SimpleTexture noise;

};

}

#endif

