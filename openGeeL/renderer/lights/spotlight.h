#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

	class ImageTexture;
	class Texture;
	class Transform;

	class SpotLight : public Light {

	public:
		SpotLight(Transform& transform, 
			vec3 diffuse, 
			float angle = 30.f, 
			float outerAngle = 5.f, 
			const std::string& name = "SpotLight");

		virtual void bind(const Shader& shader,
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		//Set light cookie for this spotlight
		void setLightCookie(ImageTexture& cookie);
		const Texture * const getLightCookie() const;

		
		//Add light cookie of this spotlight to given shader.
		//Note: 'addShadowmap' has same effect
		void addLightCookie(Shader& shader, const std::string& name);
		virtual void addShadowmap(Shader& shader, const std::string& name);

		float getAngle() const;
		virtual LightType getLightType() const;

	private:
		ImageTexture* lightCookie;
		float angle, outerAngle;

	};


	inline float SpotLight::getAngle() const {
		return angle;
	}

	inline LightType SpotLight::getLightType() const {
		return LightType::Spot;
	}

}

#endif
