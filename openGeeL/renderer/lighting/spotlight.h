#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

	class Transform;

	class SpotLight : public Light {

	public:
		SpotLight(Transform& transform, vec3 diffuse, 
			float angle = 30.f, float outerAngle = 5.f, float shadowBias = 0.003f);

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, std::string name) const;
		virtual void forwardBind(const Shader& shader, std::string name, std::string transformName) const;
		virtual void computeLightTransform();

	protected:
		virtual bool adaptShadowmapResolution(float distance);

	private:
		float angle, outerAngle;

	};
}

#endif
