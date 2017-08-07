#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

	class ImageTexture;
	class Texture;
	class Transform;

	class SpotLight : public Light {

	public:
		SpotLight(Transform& transform, vec3 diffuse, float angle = 30.f, 
			float outerAngle = 5.f, const std::string& name = "SpotLight");

		virtual void bind(const RenderShader& shader,
			const std::string& name, ShaderTransformSpace space, const Camera* const camera = nullptr) const;

		//Set light cookie for this spotlight
		void setLightCookie(ImageTexture& cookie);
		const Texture * const getLightCookie() const;

		//Add light cookie of this spotlight to given shader.
		void addLightCookie(RenderShader& shader, const std::string& name);

		float getAngle() const;

	private:
		ImageTexture* lightCookie;
		float angle, outerAngle;

	};


	inline float SpotLight::getAngle() const {
		return angle;
	}

}

#endif
