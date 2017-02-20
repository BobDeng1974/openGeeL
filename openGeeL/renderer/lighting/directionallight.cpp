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

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, vec3 ambient, 
		float shadowBias) : Light(transform, diffuse, ambient, shadowBias) {
	
		setResolution(ShadowmapResolution::High);
	}


	void DirectionalLight::deferredBind(const RenderScene& scene, const Shader& shader, int index, string name) const {
		Light::deferredBind(scene, shader, index, name);

		string location = name + "[" + to_string(index) + "].";
		shader.setVector3(location + "direction", scene.GetOriginInViewSpace() - scene.TranslateToViewSpace(transform.forward));
		shader.setMat4(location + "lightTransform", lightTransform);
	}

	void DirectionalLight::forwardBind(const Shader& shader, int index, string name) const {
		Light::forwardBind(shader, index, name);

		string location = name + "[" + to_string(index) + "].";
		shader.setVector3(location + "direction", transform.forward);

		location = "direLightMatrix[" + to_string(index) + "]";
		shader.setMat4(location, lightTransform);
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
		mat4 view = lookAt(transform.forward * far, vec3(0.f), vec3(0.f, 1.f, 0.f));
		
		lightTransform = projection * view;
	}

	void DirectionalLight::computeLightTransformExt(const ScreenInfo& info, vec3 offset) {
		float far = fmaxf(info.CTdepth, fmaxf(info.BLdepth, fmaxf(info.BRdepth, fmaxf(info.TLdepth, info.TRdepth))));
		far = fmaxf(8.f, fminf(50.f, far));

		float a = (shadowmapWidth / 500.f) * far;
		mat4 projection = ortho(-a, a, -a, a, 1.0f, 4.f * far);
		mat4 view = lookAt(transform.forward * far + offset, offset, vec3(0.f, 1.f, 0.f));

		lightTransform = projection * view;
	}

	float DirectionalLight::getIntensity(glm::vec3 point) const {
		return 1.f;
	}

	bool DirectionalLight::adaptShadowmapResolution(float distance) {
		return false;
	}
}