#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

class SpotLight : public Light {

public:
	SpotLight(vec3 position, vec3 direction, vec3 diffuse, vec3 specular, vec3 ambient, float intensity = 1.f, float angle = 30.f, float outerAngle = 5.f,
		float constant = 1.f, float linear = 0.09f, float quadratic = 0.032f);

	virtual void bind(const Shader& shader, int index, string name) const;
	virtual void computeLightTransform();

private:
	vec3 position, direction;
	float angle, outerAngle;
	float constant, linear, quadratic;
};

}

#endif
