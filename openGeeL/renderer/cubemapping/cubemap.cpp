#define GLEW_STATIC
#include <glew.h>
#include "../texturing/texture.h"
#include "../shader/rendershader.h"
#include "cubemap.h"

#define GL_TEXTURE_CUBE_MAP 0x8513

namespace geeL {

	void CubeMap::draw(const RenderShader& shader, std::string name) const {
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(shader.getProgram(), name.c_str()), 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, getID());
	}

	void CubeMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(*texture, name + "albedo");
	}

	void CubeMap::remove(RenderShader& shader, unsigned int id) const {
		shader.removeMap(id);
	}

	unsigned int CubeMap::getID() const {
		return texture->getID();
	}
	
	const TextureCube & CubeMap::getTexture() const {
		return *texture;
	}

	TextureCube & CubeMap::getTexture() {
		return *texture;
	}
}