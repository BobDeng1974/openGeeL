#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <vector>
#include "light.h"

namespace geeL {

	class PointLight : public Light {

	public:
		PointLight(Transform& transform, vec3 diffuse, float cutoff = 0.01f, 
			const std::string& name = "Pointlight");

		virtual void bind(const Shader& shader, const std::string& name, ShaderTransformSpace space, 
			const Camera* const camera = nullptr) const;

		virtual void setMapIndex(unsigned int index, LightMapType type);
		virtual LightMapContainer getMaps() const;
		virtual const ITexture* const getMap(LightMapType type);

		virtual std::string getShadowmapContainerName() const;
		virtual LightType getLightType() const;

		float getVolumetricStrength() const;
		float getVolumetricDensity() const;

		void setVolumetricStrength(float value);
		void setVolumetricDensity(float value);

	private:
		float cutoff, volStrength, volDensity;

	};


	

	inline std::string PointLight::getShadowmapContainerName() const {
		return "shadowCubes";
	}

	inline LightType PointLight::getLightType() const {
		return LightType::Point;
	}


	
}

#endif
