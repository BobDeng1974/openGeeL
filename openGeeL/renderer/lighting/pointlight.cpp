#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <string>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "../meshes/meshrenderer.h"
#include "../scene.h"
#include "pointlight.h"

using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, 
		float intensity, float constant, float linear, float quadratic)
		: 
		Light(transform, diffuse, specular, ambient, intensity), 
		constant(constant), linear(linear), quadratic(quadratic) {
	
		lightTransforms.reserve(6);
	}


	void PointLight::bind(const Shader& shader, int index, string name) const {
		Light::bind(shader, index, name);

		GLuint program = shader.program;
		std::string location = name + "[" + std::to_string(index) + "].";

		glUniform3f(glGetUniformLocation(program, (location + "position").c_str()), 
			transform.position.x, transform.position.y, transform.position.z);
		glUniform1f(glGetUniformLocation(program, (location + "constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(program, (location + "linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(program, (location + "quadratic").c_str()), quadratic);
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
		glBindTexture(GL_TEXTURE_2D, 0);

		//Bind depth map to frame buffer (the shadow map)
		glGenFramebuffers(1, &shadowmapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowmapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void PointLight::renderShadowmap(const RenderScene& scene, const Shader& shader) {
		shader.use();

		//Write light transforms of cubemap faces into shader
		computeLightTransform();
		for (int i = 0; i < 6; i++) {
			string name = "lightTransforms[" + std::to_string(i) + "]";
			glUniformMatrix4fv(glGetUniformLocation(shader.program, name.c_str()), 1, GL_FALSE,
				glm::value_ptr(lightTransforms[i]));
		}

		glViewport(0, 0, shadowmapWidth, shadowmapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowmapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		scene.drawObjects(shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	
	void PointLight::computeLightTransform() {
		mat4 projection = glm::perspective(90.f, 1.f, 1.0f, 50.f);

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
	
}