#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

class Transform;

class SpotLight : public Light {

public:
	SpotLight(Transform& transform, vec3 diffuse, vec3 specular, 
		float angle = 30.f, float outerAngle = 5.f, float shadowBias = 0.0005f);

	virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
	virtual void forwardBind(const Shader& shader, int index, std::string name) const;
	virtual void computeLightTransform();

private:
	float angle, outerAngle;

};

}

#endif
