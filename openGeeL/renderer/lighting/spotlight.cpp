#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "spotlight.h"

using namespace std;
using namespace glm;


namespace geeL {

	SpotLight::SpotLight(Transform& transform, vec3 diffuse, 
		float angle, float outerAngle, float shadowBias, float farPlane, std::string name)
			: Light(transform, diffuse, shadowBias, name), 
				angle(angle), outerAngle(outerAngle), farPlane(farPlane), lightCookie(nullptr) {
	
		setResolution(ShadowmapResolution::Adaptive);
	}


	void SpotLight::deferredBind(const RenderScene& scene, const Shader& shader, string name) const {
		Light::deferredBind(scene, shader, name);

		shader.setVector3(name + "position", scene.TranslateToViewSpace(transform.getPosition()));
		shader.setVector3(name + "direction",
			scene.TranslateToViewSpace(transform.getForwardDirection()) - scene.GetOriginInViewSpace());
		shader.setFloat(name + "angle", angle);
		shader.setFloat(name + "outerAngle", outerAngle);
		shader.setMat4(name + "lightTransform", lightTransform);
		shader.setFloat(name + "useCookie", (lightCookie != nullptr));
	}

	void SpotLight::forwardBind(const Shader& shader, string name, string transformName) const {
		Light::forwardBind(shader, name, transformName);

		shader.setVector3(name + "position", transform.getPosition());
		shader.setVector3(name + "direction", transform.getForwardDirection());
		shader.setFloat(name + "angle", angle);
		shader.setFloat(name + "outerAngle", outerAngle);
		shader.setFloat(name + "useCookie", (lightCookie != nullptr));

		shader.setMat4(transformName, lightTransform);
	}

	void SpotLight::setLightCookie(SimpleTexture& cookie) {
		lightCookie = &cookie;
	}

	unsigned int SpotLight::getLightCookieID() const {
		if (lightCookie != nullptr)
			return lightCookie->getID();

		return 0;
	}

	void SpotLight::addLightCookie(Shader& shader, string name) {
		if(lightCookie != nullptr)
			shader.addMap(lightCookie->getID(), name, GL_TEXTURE_2D);
	}

	void SpotLight::computeLightTransform() {
		float fov = glm::degrees(angle);
		mat4 projection = glm::perspective(fov, 1.f, 1.0f, farPlane);
		mat4 view = lookAt(transform.getPosition(), transform.getPosition() + 
			transform.getForwardDirection(), transform.getUpDirection());

		lightTransform = projection * view;
	}

	bool SpotLight::adaptShadowmapResolution(float distance) {

		bool changed = false;
		if (distance < 1.f) {

			int resolution = 1024; //^= ShadowmapResolution::VeryHigh
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.5f;
				changed = true;
			}
		}
		else if (distance < 6.f) {

			int resolution = 768;
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.5f;
				changed = true;
			}
		}
		else if (distance < 10.f) {

			int resolution = 512; //^= ShadowmapResolution::High
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.5f;
				changed = true;
			}
		}
		else if (distance < 15.f) {

			int resolution = 256; //^= ShadowmapResolution::Medium
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias;
				changed = true;
			}
		}
		else {

			int resolution = 128; //^= ShadowmapResolution::Small
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 1.5f;
				changed = true;
			}
		}

		return changed;
	}
}