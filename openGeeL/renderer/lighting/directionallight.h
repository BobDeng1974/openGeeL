#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"

namespace geeL {

class DirectionalLight : public Light {

public:

	DirectionalLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, float intensity = 1.f);

	virtual void bind(const Shader& shader, int index, string name) const;
	virtual void computeLightTransform();

};

}

#endif

