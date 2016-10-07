#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

class DefaultPostProcess : public PostProcessingEffect {

public:
	DefaultPostProcess();

	virtual void draw();
};

}

#endif

