#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "spotlight.h"

using namespace glm;


namespace geeL {

	SpotLight::SpotLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, float intensity, float angle, 
		float outerAngle, float constant, float linear, float quadratic) 
		: 
		Light(transform, diffuse, specular, ambient, intensity),
		angle(angle), outerAngle(outerAngle),
		constant(constant), linear(linear), quadratic(quadratic) {}


	void SpotLight::bind(const Shader& shader, int index, string name) const {
		Light::bind(shader, index, name);

		GLuint program = shader.program;
		std::string location = name + "[" + std::to_string(index) + "].";

		glUniform3f(glGetUniformLocation(program, (location + "position").c_str()), 
			transform.position.x, transform.position.y, transform.position.z);
		
		glUniform3f(glGetUniformLocation(program, (location + "direction").c_str()), 
			transform.forward.x, transform.forward.y, transform.forward.z);
		
		glUniform1f(glGetUniformLocation(program, (location + "angle").c_str()), angle);
		glUniform1f(glGetUniformLocation(program, (location + "outerAngle").c_str()), outerAngle);
		glUniform1f(glGetUniformLocation(program, (location + "constant").c_str()), constant);
		glUniform1f(glGetUniformLocation(program, (location + "linear").c_str()), linear);
		glUniform1f(glGetUniformLocation(program, (location + "quadratic").c_str()), quadratic);

		glUniformMatrix4fv(glGetUniformLocation(shader.program, "lightTransform"), 1, GL_FALSE,
			glm::value_ptr(lightTransform));
	}

	void SpotLight::computeLightTransform() {
		float fov = glm::degrees(angle);
		mat4 projection = glm::perspective(fov, 1.f, 1.0f, 50.f);
		mat4 view = lookAt(transform.position, transform.forward, transform.up);

		lightTransform = projection * view;
	}
}