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

	PointLight::PointLight(Transform& transform, vec3 diffuse, vec3 specular, float shadowBias)
		: Light(transform, diffuse, specular, shadowBias), farPlane(50.f) {
	
		lightTransforms.reserve(6);
		initLightTransform();

		setResolution(ShadowmapResolution::Adaptive);
	}


	void PointLight::deferredBind(const RenderScene& scene, const Shader& shader, int index, string name) const {
		Light::deferredBind(scene, shader, index, name);

		std::string location = name + "[" + std::to_string(index) + "].";
		shader.setVector3(location + "position", scene.TranslateToViewSpace(transform.position));
		shader.setFloat(location + "farPlane", farPlane);
	}

	void PointLight::forwardBind(const Shader& shader, int index, string name) const {
		Light::forwardBind(shader, index, name);

		std::string location = name + "[" + std::to_string(index) + "].";
		shader.setVector3(location + "position", transform.position);
		shader.setFloat(location + "farPlane", farPlane);
	}

	void PointLight::initShadowmap() {

		//Generate depth cube map texture
		glGenTextures(1, &shadowmapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowmapID);

		//Write faces of the cubemap
		for (int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
				shadowmapWidth, shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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

	void PointLight::renderShadowmap(const RenderScene& scene, const Shader& shader) {
		shader.use();

		//Write light transforms of cubemap faces into shader
		computeLightTransform();
		for (int i = 0; i < 6; i++) {
			string name = "lightTransforms[" + to_string(i) + "]";
			shader.setMat4(name, lightTransforms[i]);
		}

		shader.setFloat("farPlane", farPlane);
		shader.setVector3("lightPosition", transform.position);

		if (resolution == ShadowmapResolution::Adaptive)
			adaptShadowmap(scene);

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
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
				dynamicBias = shadowBias * 0.6f;
				changed = true;
			}
		}
		else if (distance < 10.f) {

			int resolution = 768;
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.7f;
				changed = true;
			}
		}
		else if (distance < 15.f) {

			int resolution = 512; //^= ShadowmapResolution::High
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias * 0.8f;
				changed = true;
			}
		}
		else if (distance < 20.f) {

			int resolution = 386;
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				dynamicBias = shadowBias;
				changed = true;
			}
		}
		else {
			int resolution = 256; //^= ShadowmapResolution::Medium
			if (shadowmapWidth != resolution) {
				setDimensions(resolution);
				changed = true;
			}

			dynamicBias = (distance > 15.f) ? shadowBias * 2.f : shadowBias * 1.2f;
		}

		return changed;
	}

	
	void PointLight::initLightTransform() {
		mat4 projection = glm::perspective(90.f, 1.f, 1.0f, farPlane);

		mat4 view = glm::lookAt(transform.position, transform.position + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.position, transform.position + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms.push_back(projection * view);
	}

	void PointLight::computeLightTransform() {
		mat4 projection = glm::perspective(90.f, 1.f, 1.0f, farPlane);

		mat4 view = glm::lookAt(transform.position, transform.position + vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[0] = projection * view;

		view = glm::lookAt(transform.position, transform.position + vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[1] = projection * view;

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f));
		lightTransforms[2] = projection * view;

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f));
		lightTransforms[3] = projection * view;

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[4] = projection * view;

		view = glm::lookAt(transform.position, transform.position + vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f));
		lightTransforms[5] = projection * view;
	}
}