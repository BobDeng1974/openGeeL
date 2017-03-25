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

	SpotLight::SpotLight(Transform& transform, vec3 diffuse, float angle, float outerAngle, const string& name)
			: Light(transform, diffuse, name), angle(angle), outerAngle(outerAngle), lightCookie(nullptr) {}


	void SpotLight::deferredBind(const RenderScene& scene, const Shader& shader, const string& name) const {
		Light::deferredBind(scene, shader, name);

		shader.setVector3(name + "position", scene.TranslateToViewSpace(transform.getPosition()));
		shader.setVector3(name + "direction",
			scene.TranslateToViewSpace(transform.getForwardDirection()) - scene.GetOriginInViewSpace());
		shader.setFloat(name + "angle", angle);
		shader.setFloat(name + "outerAngle", outerAngle);
		shader.setFloat(name + "useCookie", (lightCookie != nullptr));
	}

	void SpotLight::forwardBind(const Shader& shader, const string& name, const string& transformName) const {
		Light::forwardBind(shader, name, transformName);

		shader.setVector3(name + "position", transform.getPosition());
		shader.setVector3(name + "direction", transform.getForwardDirection());
		shader.setFloat(name + "angle", angle);
		shader.setFloat(name + "outerAngle", outerAngle);
		shader.setFloat(name + "useCookie", (lightCookie != nullptr));

	}

	void SpotLight::setLightCookie(SimpleTexture& cookie) {
		lightCookie = &cookie;
	}

	unsigned int SpotLight::getLightCookieID() const {
		if (lightCookie != nullptr)
			return lightCookie->getID();

		return 0;
	}

	void SpotLight::addLightCookie(Shader& shader, const string& name) {
		if(lightCookie != nullptr)
			shader.addMap(lightCookie->getID(), name, GL_TEXTURE_2D);
	}


}