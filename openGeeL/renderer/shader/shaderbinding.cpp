#include "../texturing/texture.h"
#include "shader.h"
#include "shaderbinding.h"

namespace geeL {

	ShaderBinding::ShaderBinding(const Shader& shader, const std::string&name)
		: shader(shader), location(shader.getLocation(name)), name(name) {}

	std::string ShaderBinding::toString() const {
		return name;
	}


	bool TextureBinding::operator== (const TextureBinding &rhs) {
		return *texture == *rhs.texture;
	}
}