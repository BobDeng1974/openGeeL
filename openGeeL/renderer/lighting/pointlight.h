#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, vec3 specular, float shadowBias = 0.007f);

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
		virtual void forwardBind(const Shader& shader, int index, std::string name) const;
		virtual void initShadowmap();
		virtual void renderShadowmap(const RenderScene& scene, const Shader& shader);
		virtual void addShadowmap(Shader& shader, std::string name);
		virtual void computeLightTransform();

	private:
		float farPlane;
		std::vector<glm::mat4> lightTransforms;
	
		void PointLight::initLightTransform();
	};
}

#endif
