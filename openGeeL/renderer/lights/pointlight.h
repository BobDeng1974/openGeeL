#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, const std::string& name = "Pointlight");

		virtual void bind(const RenderShader& shader, const std::string& name, ShaderTransformSpace space, 
			const Camera* const camera = nullptr) const;

	};
}

#endif
