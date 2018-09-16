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

		//Set light cookie for this spotlight
		void setLightCookie(memory::MemoryObject<ImageTexture> cookie);
		const ITexture* const getLightCookie() const;

		
		//Add light cookie of this spotlight to given shader.
		//Note: 'addShadowmap' has same effect
		void addLightCookie(Shader& shader, const std::string& name);
		virtual void bindShadowmap(Shader& shader, const std::string& name);

		float getAngle() const;
		float getAngleDegree() const;
		void  setAngleDegree(float value);

		float getOuterAngleDegree() const;
		void  setOuterAngleDegree(float value);

		virtual LightType getLightType() const;

	private:
		memory::MemoryObject<ImageTexture> lightCookie;
		float angle, outerAngle, cutoff;

	};


	inline LightType SpotLight::getLightType() const {
		return LightType::Spot;
	}

}

#endif
