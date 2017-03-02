#include "../shader/shader.h"
#include "cubemap.h"

#define GL_TEXTURE_CUBE_MAP 0x8513

namespace geeL {

	void CubeMap::add(Shader& shader, std::string name) const {
		shader.addMap(id, name, GL_TEXTURE_CUBE_MAP);
	}

	unsigned int CubeMap::getID() const {
		return id;
	}

}