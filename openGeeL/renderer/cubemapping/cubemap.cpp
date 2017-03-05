#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "cubemap.h"

#define GL_TEXTURE_CUBE_MAP 0x8513

namespace geeL {

	void CubeMap::bind(const Shader& shader, std::string name) const {

		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.program, name.c_str()), 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}

	void CubeMap::add(Shader& shader, std::string name) const {
		shader.addMap(id, name, GL_TEXTURE_CUBE_MAP);
	}

	unsigned int CubeMap::getID() const {
		return id;
	}

}