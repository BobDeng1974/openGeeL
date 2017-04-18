#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../shader/sceneshader.h"
#include "../texturing/simpletexture.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "spotlight.h"

using namespace std;
using namespace glm;


namespace geeL {

	SpotLight::SpotLight(Transform& transform, vec3 diffuse, float angle, float outerAngle, const string& name)
			: Light(transform, diffuse, name), angle(angle), outerAngle(outerAngle), lightCookie(nullptr) {}


	void SpotLight::bind(const SceneCamera& camera, const Shader& shader, const string& name, ShaderTransformSpace space) const {
		Light::bind(camera, shader, name, space);

		switch (space) {
			case ShaderTransformSpace::View:
				shader.setVector3(name + "position", camera.TranslateToViewSpace(transform.getPosition()));
				shader.setVector3(name + "direction",
					camera.TranslateToViewSpace(transform.getForwardDirection()) - camera.GetOriginInViewSpace());
				break;
			case ShaderTransformSpace::World:
				shader.setVector3(name + "position", transform.getPosition());
				shader.setVector3(name + "direction", transform.getForwardDirection());
				break;
		}

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