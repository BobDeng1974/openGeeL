#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, const std::string& name = "Pointlight");

		virtual void bind(const Shader& shader, const std::string& name, ShaderTransformSpace space, 
			const Camera* const camera = nullptr) const;

		virtual LightType getLightType() const;
		float getLightRadius() const;

	};


	inline LightType PointLight::getLightType() const {
		return LightType::Point;
	}

}

#endif
