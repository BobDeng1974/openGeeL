#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, float shadowBias = 0.006f);

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, std::string name) const;
		virtual void forwardBind(const Shader& shader, std::string name, std::string transformName) const;
		virtual void initShadowmap();
		virtual void renderShadowmap(const RenderScene& scene, const Shader& shader);
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
