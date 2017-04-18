#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, const std::string& name = "Pointlight");

		virtual void bind(const SceneCamera& camera, const Shader& shader,
			const std::string& name, ShaderTransformSpace space) const;

	};
}

#endif
