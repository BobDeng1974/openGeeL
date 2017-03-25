#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, float shadowBias = 0.006f, 
			float farPlane = 100.f, const std::string& name = "Pointlight");

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, const std::string& name) const;
		virtual void forwardBind(const Shader& shader, const std::string& name, const std::string& transformName) const;



	
	};
}

#endif
