#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../meshes/meshrenderer.h"
#include "../scene.h"
#include "pointlight.h"

using namespace std;
using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, 
		float intensity, float shadowBias, float constant, float linear, float quadratic)
		: 
		Light(transform, diffuse, specular, ambient, intensity, shadowBias), 
		constant(constant), linear(linear), quadratic(quadratic), farPlane(50.f) {
	
		lightTransforms.reserve(6);
		initLightTransform();
	}


	void PointLight::deferredBind(const Shader& shader, int index, string name) const {
		Light::deferredBind(shader, index, name);

		std::string location = name + "[" + std::to_string(index) + "].";
		shader.setVector3(location + "position", transform.position);
		shader.setFloat(location + "constant", constant);
		shader.setFloat(location + "linear", linear);
		shader.setFloat(location + "quadratic", quadratic);
		shader.setFloat(location + "farPlane", farPlane);
	}

	void PointLight::forwardBind(const Shader& shader, int index, string name) const {
		deferredBind(shader, index, name);
	}

	void PointLight::initShadowmap() {
		shadowmapHeight = shadowmapWidth = 512;

		//Generate depth cube map texture
		glGenTextures(1, &shadowmapID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, shadowmapID);

		//Write faces of the cubemap
		for(int i = 0; i < 6; i++)
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
		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
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