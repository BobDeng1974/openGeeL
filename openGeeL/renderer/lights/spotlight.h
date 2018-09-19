#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "memory/memoryobject.h"
#include "light.h"

namespace geeL {

	class ImageTexture;
	class ITexture;
	class Transform;

	class SpotLight : public Light {

	public:
		SpotLight(Transform& transform,
			vec3 diffuse,
			float angle = 30.f,
			float outerAngle = 1.f,
			float cutoff = 0.01f,
			const std::string& name = "SpotLight");

		virtual void bind(const Shader& shader,
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		virtual void setMapIndex(unsigned int index, LightMapType type);
		virtual LightMapContainer getMaps() const;
		virtual const ITexture* const getMap(LightMapType type);

		void setLightCookie(memory::MemoryObject<ImageTexture> cookie);
		const ITexture* const getLightCookie() const;

		float getAngle() const;
		float getAngleDegree() const;
		void  setAngleDegree(float value);

		float getOuterAngleDegree() const;
		void  setOuterAngleDegree(float value);

		virtual std::string getShadowmapContainerName() const;
		virtual LightType getLightType() const;

	private:
		memory::MemoryObject<ImageTexture> lightCookie;
		unsigned int cookieIndex;
		float angle, outerAngle, cutoff;

	};


	inline std::string SpotLight::getShadowmapContainerName() const {
		return "shadowMaps";
	}

	inline LightType SpotLight::getLightType() const {
		return LightType::Spot;
	}

}

#endif
