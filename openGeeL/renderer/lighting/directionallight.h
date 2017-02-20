#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"

namespace geeL {

	struct ScreenInfo;

	class DirectionalLight : public Light {

	public:
		DirectionalLight(Transform& transform, vec3 diffuse, vec3 specular, float shadowBias = 0.0005f);

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, int index, std::string name) const;
		virtual void forwardBind(const Shader& shader, int index, std::string name) const;

		virtual void renderShadowmap(const RenderScene& scene, const Shader& shader);
		virtual void computeLightTransform();
		virtual void computeLightTransformExt(const ScreenInfo& info, glm::vec3 offset);
		virtual float getIntensity(glm::vec3 point) const;

	protected:
		virtual bool adaptShadowmapResolution(float distance);

	};
}

#endif

