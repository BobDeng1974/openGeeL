#ifndef DEFAULTPOST_H
#define DEFAULTPOST_H

#include "postprocessing.h"

namespace geeL {

class DefaultPostProcess : public PostProcessingEffect {

public:
	DefaultPostProcess();

protected:
	virtual void bindValues();

};

}

#endif

