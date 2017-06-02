#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"

namespace geeL {

	struct ScreenInfo;

	class DirectionalLight : public Light {

	public:
		DirectionalLight(Transform& transform, vec3 diffuse, const std::string& name = "DirectionalLight");

		virtual void bind(const Camera& camera, const RenderShader& shader,
			const std::string& name, ShaderTransformSpace space) const;

		virtual float getIntensity(glm::vec3 point) const;

	};
}

#endif

