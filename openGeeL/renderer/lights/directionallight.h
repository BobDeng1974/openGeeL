#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "light.h"

namespace geeL {

	struct ScreenInfo;

	class DirectionalLight : public Light {

	public:
		DirectionalLight(Transform& transform, vec3 diffuse, const std::string& name = "DirectionalLight");

		virtual void bind(const Shader& shader,
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		virtual void setMapIndex(unsigned int index, LightMapType type);
		virtual LightMapContainer getMaps() const;
		virtual const ITexture* const getMap(LightMapType type);

		virtual float getAttenuation(glm::vec3 point) const;

		virtual std::string getShadowmapContainerName() const;
		virtual LightType getLightType() const;

	};


	inline std::string DirectionalLight::getShadowmapContainerName() const {
		return "shadowMaps";
	}

	inline LightType DirectionalLight::getLightType() const {
		return LightType::Directional;
	}

}

#endif

