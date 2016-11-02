#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

class DefaultPostProcess : public PostProcessingEffect {

public:
	float exposure;

	DefaultPostProcess(float exposure = 1.f);

protected:
	virtual void bindValues();

};

}

#endif

