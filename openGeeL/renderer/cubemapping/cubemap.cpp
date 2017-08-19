#define GLEW_STATIC
#include <glew.h>
#include "texturing/texture.h"
#include "shader/rendershader.h"
#include "cubemap.h"

#define GL_TEXTURE_CUBE_MAP 0x8513

namespace geeL {

	void CubeMap::bindMap(const RenderShader& shader, std::string name) const {
		glActiveTexture(GL_TEXTURE1);
		shader.bind<int>(name, 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, getID());
	}


	void CubeMap::bind(const Camera& camera, const RenderShader& shader, 
		const std::string& name, ShaderTransformSpace space) const {

		shader.bind<int>(name + "global", true);
	}

	void CubeMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(*texture, name + "albedo");
	}

	void CubeMap::remove(RenderShader& shader) const {
		shader.removeMap(*texture);
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