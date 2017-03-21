#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../meshes/meshrenderer.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "pointlight.h"

using namespace std;
using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, float shadowBias, float farPlane, std::string name)
		: Light(transform, diffuse, shadowBias, name), farPlane(farPlane) {
	
		lightTransforms.reserve(6);
		initLightTransform();

		setResolution(ShadowmapResolution::Adaptive);
	}


	void PointLight::deferredBind(const RenderScene& scene, const Shader& shader, string name) const {
		Light::deferredBind(scene, shader, name);

		shader.setVector3(name + "position", scene.TranslateToViewSpace(transform.getPosition()));
		shader.setFloat(name + "farPlane", farPlane);
	}

	void PointLight::forwardBind(const Shader& shader, string name, string transformName) const {
		Light::forwardBind(shader, name, transformName);

		shader.setVector3(name + "position", transform.getPosition());
		shader.setFloat(name + "farPlane", farPlane);
	}

	void PointLight::initShadowmap() {

		//Generate depth cube map texture
		glGenTextures(1, &shadowmapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowmapID);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				shadowmapWidth, shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &shadowmapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PointLight::addShadowmap(Shader& shader, std::string name) {
		shader.addMap(shadowmapID, name, GL_TEXTURE_CUBE_MAP);
	}

	void PointLight::renderShadowmap(const Camera& camera, 
		std::function<void(const Shader&)> renderCall, const Shader& shader) {
		
		//Write light transforms of cubemap faces into shader
		computeLightTransform();

		shader.use();
		for (int i = 0; i < 6; i++) {
			string name = "lightTransforms[" + to_string(i) + "]";
			shader.setMat4(name, lightTransforms[i]);
		}

		shader.setFloat("farPlane", farPlane);
		shader.setVector3("lightPosition", transform.getPosition());

		if (resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(camera);

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		renderCall(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PointLight::bindShadowmapResolution() const {
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowmapID);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				shadowmapWidth, shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	bool PointLight::adaptShadowmapResolution(float distance) {

		bool changed = false;
		if (distance < 4.f) {

			int resolution = 1024; //^= ShadowmapResolution::VeryHigh
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.2f;
				changed = true;
			}
		}
		else if (distance < 10.f) {

			int resolution = 768;
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.3f;
				changed = true;
			}
		}
		else if (distance < 15.f) {

			int resolution = 512; //^= ShadowmapResolution::High
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.4f;
				changed = true;
			}
		}
		else if (distance < 20.f) {

			int resolution = 386;
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.5f;
				changed = true;
			}
		}
		else {
			int resolution = 256; //^= ShadowmapResolution::Medium
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				changed = true;
			}

			dynamicBias = (distance > 15.f) ? shadowBias * 0.8f : shadowBias * 0.6f;
		}

		return changed;
	}

	
	void PointLight::initLightTransform() {
		mat4 projection = glm::perspective(90.f, 1.f, 1.0f, farPlane);

		mat4 view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);
	}

	void PointLight::computeLightTransform() {
		mat4 projection = glm::perspective(90.f, 1.f, 1.0f, farPlane);

		mat4 view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[0] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[1] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
		lightTransforms[2] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f));
		lightTransforms[3] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[4] = projection * view;

		view = glm::lookAt(transform.getPosition(), transform.getPosition() + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[5] = projection * view;
	}
}