#define GLEW_STATIC
#include <glew.h>
#include <string>
#include "../shader/shader.h"
#include "light.h"

namespace geeL {

	void Light::bind(const Shader& shader, int index, string name) const {
		GLuint program = shader.program;

		std::string location = name + "[" + std::to_string(index) + "].";

		glUniform3f(glGetUniformLocation(program, (location + "diffuse").c_str()), diffuse.x, diffuse.y, diffuse.z);
		glUniform3f(glGetUniformLocation(program, (location + "specular").c_str()), specular.x, specular.y, specular.z);
		glUniform3f(glGetUniformLocation(program, (location + "ambient").c_str()), ambient.x, ambient.y, ambient.z);

		glUniform1f(glGetUniformLocation(program, (location + "intensity").c_str()), intensity);
	}

	const int Light::getShadowMapID() const {
		return shadowmapID;
	}

	const int Light::getShadowMapFBO() const {
		return shadowmapFBO;
	}
}