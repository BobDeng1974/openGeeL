#define GLEW_STATIC
#include <glew.h>
#include <vector>
#include "texturing/textureparams.h"
#include "texturing/texture.h"
#include "shader.h"
#include "bindingstack.h"
#include <iostream>

namespace geeL {

	std::vector<unsigned int> bindings(TextureBindingStack::MAX_TEXTURE_BINDINGS, 0);


	void TextureBindingStack::bindTextures(const Shader& shader, unsigned int offset) {

		shader.iterateTextures([&offset, &shader](const TextureBinding& binding) {
			unsigned int& b = bindings[binding.offset];

			if (shader.ignoreOptimisations || b != binding.texture->getID()) {
				binding.texture->bind(offset + binding.offset);
				b = binding.texture->getID();
			}
		});
	}

	void TextureBindingStack::bindSingleTexture(unsigned int GID, unsigned int ID, const Shader& shader,
		unsigned int offset, const std::string& name, TextureType type) {

		glActiveTexture(GL_TEXTURE0 + offset);

		TextureParameters::unbind(offset);
		shader.bind<int>(name, offset);
		glBindTexture((int)type, GID);

		bindings[offset] = ID;
	}

}