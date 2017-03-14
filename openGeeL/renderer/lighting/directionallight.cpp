#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "../utility/gbuffer.h"
#include "directionallight.h"
#include <iostream>

using namespace std;
using namespace glm;

namespace geeL {

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, float shadowBias, std::string name)
		: Light(transform, diffuse, shadowBias, name) {
	
		setResolution(ShadowmapResolution::High);
	}


	void DirectionalLight::deferredBind(const RenderScene& scene, const Shader& shader, string name) const {
		Light::deferredBind(scene, shader, name);

		shader.setVector3(name + "direction", scene.GetOriginInViewSpace() - scene.TranslateToViewSpace(transform.getForwardDirection()));
		shader.setMat4(name + "lightTransform", lightTransform);
	}

	void DirectionalLight::forwardBind(const Shader& shader, string name, string transformName) const {
		Light::forwardBind(shader, name, transformName);

		shader.setVector3(name + "direction", transform.getForwardDirection());
		shader.setMat4(transformName, lightTransform);
	}

	void DirectionalLight::renderShadowmap(const RenderScene& scene, const Shader& shader) {
		shader.use();

		shader.setMat4("lightTransform", lightTransform);
		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DirectionalLight::computeLightTransform() {
		float far = 50.f;
		float a = shadowmapWidth / 50.f;
		mat4 projection = ortho(-a, a, -a, a, 1.0f, 2 * far);
		mat4 view = lookAt(transform.getForwardDirection() * far, vec3(0.f), vec3(0.f, 1.f, 0.f));
		
		lightTransform = projection * view;
	}

	void DirectionalLight::computeLightTransformExt(const ScreenInfo& info, vec3 offset) {
		float far = fmaxf(info.CTdepth, fmaxf(info.BLdepth, fmaxf(info.BRdepth, fmaxf(info.TLdepth, info.TRdepth))));
		far = fmaxf(8.f, fminf(50.f, far));

		float a = (shadowmapWidth / 500.f) * far;
		mat4 projection = ortho(-a, a, -a, a, 1.0f, 4.f * far);
		mat4 view = lookAt(transform.getForwardDirection() * far + offset, offset, vec3(0.f, 1.f, 0.f));

		lightTransform = projection * view;
	}

	float DirectionalLight::getIntensity(glm::vec3 point) const {
		return 1.f;
	}

	bool DirectionalLight::adaptShadowmapResolution(float distance) {
		return false;
	}
}