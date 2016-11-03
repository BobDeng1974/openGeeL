#ifndef SIMPLEBLUR_H
#define SIMPLEBLUR_H

#include "postprocessing.h"

namespace geeL {

//Simple (evenly distributed) blur
class SimpleBlur : public PostProcessingEffect {

public:
	SimpleBlur(unsigned int strength = 1);

protected:
	virtual void bindValues();

private:
	unsigned int amount;

};

}

#endif