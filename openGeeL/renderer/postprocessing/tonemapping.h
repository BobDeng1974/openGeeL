#ifndef TONEMAPPING_H
#define TONEMAPPING_H

#include "postprocessing.h"


namespace geeL {

//Simple Reinhardt tone mapping
class ToneMapping : public PostProcessingEffect {

public:
	float exposure;

	ToneMapping(float exposure);

protected:
	virtual void bindValues();

};

}

#endif

