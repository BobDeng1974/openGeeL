#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/simpletexture.h"
#include "postprocessing.h"

namespace geeL {

class SSAO : public PostProcessingEffect {

public:
	SSAO();

protected:
	virtual void bindValues();

private:
	unsigned int sampleCount = 64;
	SimpleTexture noiseTexture;
	std::vector<glm::vec3> kernel;
	std::vector<glm::vec3> noise;
	

};

}

#endif