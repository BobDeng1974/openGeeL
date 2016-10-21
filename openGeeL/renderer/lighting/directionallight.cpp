#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "directionallight.h"
#include "../scene.h"

using namespace glm;


namespace geeL {

	DirectionalLight::DirectionalLight(vec3 direction, vec3 diffuse, vec3 specular, vec3 ambient, float intensity)
		: Light(diffuse, specular, ambient, intensity), direction(direction) {}

	void DirectionalLight::bind(const Shader& shader, int index, string name) const {
		Light::bind(shader, index, name);

		GLuint program = shader.program;
		std::string location = name + "[" + std::to_string(index) + "].";
		glUniform3f(glGetUniformLocation(program, (location + "direction").c_str()), 
			direction.x, direction.y, direction.z);

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "lightTransform"), 1, GL_FALSE,
			glm::value_ptr(lightTransform));
	}

	void DirectionalLight::initShadowmap() {
		shadowmapHeight = 1024;
		shadowmapWidth = 1024;

		//Generate depth map texture
		GLuint depthMap;
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			shadowmapWidth, shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		shadowmapID = depthMap;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &shadowmapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DirectionalLight::addShadowmap(Shader& shader) {
		shader.addMap(shadowmapID, "shadowMap");
	}

	void DirectionalLight::renderShadowmap(const RenderScene& scene, const Shader& shader) {
		shader.use();
		//Write light transform into shader
		mat4 trans = computeLightTransform();
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "lightTransform"), 1, GL_FALSE,
			glm::value_ptr(trans));

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	mat4 DirectionalLight::computeLightTransform() {
		mat4 projection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.f);
		mat4 view = lookAt(direction, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
		lightTransform = projection * view;

		return lightTransform;
	}
}