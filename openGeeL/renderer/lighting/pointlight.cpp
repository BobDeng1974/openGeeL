#define GLEW_STATIC
#include <glew.h>
#include "pointlight.h"
#include "../shader/shader.h"
#include <string>

namespace geeL {

	PointLight::PointLight(vec3 position, vec3 diffuse, vec3 specular, vec3 ambient, float constant, float linear, float quadratic, float intensity)
		: Light(diffuse, specular, ambient, intensity), position(position), constant(constant), linear(linear), quadratic(quadratic) {}


	void PointLight::bind(const Shader& shader, int index) const {
		GLuint program = shader.program;

		std::string location = "pointLights[" + std::to_string(index) + "].";

		glUniform3f(glGetUniformLocation(program, (location + "position").c_str()), position.x, position.y, position.z);
		glUniform3f(glGetUniformLocation(program, (location + "diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
		glUniform3f(glGetUniformLocation(program, (location + "specular").c_str()), specular.x, specular.y, specular.z);
		glUniform3f(glGetUniformLocation(program, (location + "ambient").c_str()), ambient.x, ambient.y, ambient.z);

		glUniform1f(glGetUniformLocation(program, (location + "constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(program, (location + "linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(program, (location + "quadratic").c_str()), quadratic);
	}
}