#define GLEW_STATIC
#include <glew.h>
#include "pointlight.h"
#include "../shader/shader.h"
#include <string>

namespace geeL {

	PointLight::PointLight(vec3 position, vec3 diffuse, vec3 specular, vec3 ambient, float intensity, float constant, float linear, float quadratic)
		: Light(diffuse, specular, ambient, intensity), position(position), constant(constant), linear(linear), quadratic(quadratic) {}


	void PointLight::bind(const Shader& shader, int index, string name) const {
		Light::bind(shader, index, name);

		GLuint program = shader.program;
		std::string location = name + "[" + std::to_string(index) + "].";

		glUniform3f(glGetUniformLocation(program, (location + "position").c_str()), position.x, position.y, position.z);
		glUniform1f(glGetUniformLocation(program, (location + "constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(program, (location + "linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(program, (location + "quadratic").c_str()), quadratic);
	}
}