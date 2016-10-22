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

	void DirectionalLight::computeLightTransform() {
		float a = shadowmapWidth / 100.f;
		mat4 projection = ortho(-a, a, -a, a, 1.0f, 50.f);
		mat4 view = lookAt(direction, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
		
		lightTransform = projection * view;
	}
}