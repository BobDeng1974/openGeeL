#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "light.h"

namespace geeL {

	class SimpleTexture;
	class Transform;

	class SpotLight : public Light {

	public:
		SpotLight(Transform& transform, vec3 diffuse, 
			float angle = 30.f, float outerAngle = 5.f, float shadowBias = 0.003f, 
			float farPlane = 100.f, const std::string& name = "SpotLight");

		virtual void deferredBind(const RenderScene& scene, const Shader& shader, const std::string& name) const;
		virtual void forwardBind(const Shader& shader, const std::string& name, const std::string& transformName) const;

		//Set light cookie for this spotlight
		void setLightCookie(SimpleTexture& cookie);
		unsigned int getLightCookieID() const;

		//Add light cookie of this spotlight to given shader.
		void addLightCookie(Shader& shader, const std::string& name);

		float getAngle() const;

	private:
		SimpleTexture* lightCookie;
		float angle, outerAngle;

	};


	inline float SpotLight::getAngle() const {
		return angle;
	}

}

#endif
