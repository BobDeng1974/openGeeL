#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"

namespace geeL {

class DirectionalLight : public Light {

public:

	DirectionalLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, 
		float intensity = 1.f, float shadowBias = 0.0005f);

	virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
	virtual void forwardBind(const Shader& shader, int index, std::string name) const;
	virtual void computeLightTransform();

};

}

#endif

