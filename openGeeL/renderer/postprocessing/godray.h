#ifndef GODRAY_H
#define GODRAY_H

#include <vec3.hpp>
#include "postprocessing.h"

namespace geeL {

class DirectionalLight;
class RenderScene;

class GodRay : public PostProcessingEffect {

public:
	GodRay(const RenderScene& scene, glm::vec3 lightPosition, unsigned int samples = 20.f);

protected:
	virtual void bindValues();

private:
	unsigned int samples;
	glm::vec3 lightPosition;
	const RenderScene& scene;

};

}

#endif