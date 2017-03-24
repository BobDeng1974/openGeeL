#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"

namespace geeL {

	struct ScreenInfo;

	class DirectionalLight : public Light {

	public:
		DirectionalLight(Transform& transform, vec3 diffuse, 
			float shadowBias = 0.0005f, const std::string& name = "DirectionalLight");

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, const std::string& name) const;
		virtual void forwardBind(const Shader& shader, const std::string& name, const std::string& transformName) const;

		virtual void renderShadowmap(const Camera& camera, 
			std::function<void(const Shader&)> renderCall, const Shader& shader);

		virtual void computeLightTransform();
		virtual void forwardScreenInfo(const ScreenInfo& info, glm::vec3 offset);
		virtual float getIntensity(glm::vec3 point) const;

	protected:
		virtual bool adaptShadowmapResolution(float distance);

	};
}

#endif

