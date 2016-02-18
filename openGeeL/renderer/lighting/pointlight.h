#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "light.h"

namespace geeL {

class PointLight : public Light {

public:

	PointLight(vec3 position, vec3 diffuse, vec3 specular, vec3 ambient, float constant = 1.f, float linear = 0.09f, float quadratic = 0.032f, float intensity = 1.f);

	virtual void bind(const Shader* shader, int index) const;

private:
	vec3 position;
	float constant, linear, quadratic;

};

}

#endif
