#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, float shadowBias = 0.006f, 
			float farPlane = 100.f, std::string name = "Pointlight");

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, std::string name) const;
		virtual void forwardBind(const Shader& shader, std::string name, std::string transformName) const;
		virtual void initShadowmap();

		virtual void renderShadowmap(const Camera& camera, 
			std::function<void(const Shader&)> renderCall, const Shader& shader);

		virtual void addShadowmap(Shader& shader, std::string name);
		virtual void computeLightTransform();

	protected:
		virtual void bindShadowmapResolution() const;
		virtual bool adaptShadowmapResolution(float distance);

	private:
		float farPlane;
		std::vector<glm::mat4> lightTransforms;
	
		void PointLight::initLightTransform();
	};
}

#endif
