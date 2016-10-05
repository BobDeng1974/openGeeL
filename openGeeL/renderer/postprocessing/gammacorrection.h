#ifndef GAMMACORRECTION_H
#define GAMMACORRECTION_H

#include "postprocessing.h"

namespace geeL {

class GammaCorrection : public PostProcessingEffect {

public:
	GammaCorrection(float gamma = 2.2f);

	virtual void draw();

private:
	float gamma;
};

}

#endif
