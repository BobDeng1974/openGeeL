#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

class Transform;

class SpotLight : public Light {

public:
	SpotLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, 
		float intensity = 1.f, float angle = 30.f, float outerAngle = 5.f, float shadowBias = 0.0005f,
		float constant = 1.f, float linear = 0.09f, float quadratic = 0.032f);

	virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
	virtual void forwardBind(const Shader& shader, int index, std::string name) const;
	virtual void computeLightTransform();

private:
	float angle, outerAngle;
	float constant, linear, quadratic;
};

}

#endif
