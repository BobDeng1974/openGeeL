#define GLEW_STATIC
#include <glew.h>
#include <string>
#include "../shader/shader.h"
#include "directionallight.h"

namespace geeL {

	DirectionalLight::DirectionalLight(vec3 direction, vec3 diffuse, vec3 specular, vec3 ambient, float intensity)
		: Light(diffuse, specular, ambient, intensity), direction(direction) {}

	void DirectionalLight::bind(const Shader& shader, int index, string name) const {
		Light::bind(shader, index, name);

		GLuint program = shader.program;
		std::string location = name + "[" + std::to_string(index) + "].";
		glUniform3f(glGetUniformLocation(program, (location + "direction").c_str()), direction.x, direction.y, direction.z);
	}
}