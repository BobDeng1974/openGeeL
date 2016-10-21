#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"
#include <mat4x4.hpp>

namespace geeL {

class DirectionalLight : public Light {

public:

	DirectionalLight(vec3 direction, vec3 diffuse, vec3 specular, vec3 ambient, float intensity = 1.f);

	virtual void bind(const Shader& shader, int index, string name) const;
	virtual glm::mat4 computeLightTransform();

private:
	vec3 direction;
	

};

}

#endif

