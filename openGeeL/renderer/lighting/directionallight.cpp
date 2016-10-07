#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../meshes/modeldrawer.h"
#include "directionallight.h"

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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//Bind depth map to frame buffer (the shadow map)
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DirectionalLight::renderShadowmap(const MeshDrawer& meshDrawer, const Shader& shader) {
		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		shader.use();
		//Write light transform into shader
		glUniformMatrix4fv(glGetUniformLocation(shader.program, "lightTransform"), 1, GL_FALSE, 
			glm::value_ptr(computeLightTransform()));

		meshDrawer.draw(shader);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	mat4 DirectionalLight::computeLightTransform() {
		mat4 projection = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
		mat4 view = lookAt(direction, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
		lightTransform = projection * view;

		return lightTransform;
	}
}