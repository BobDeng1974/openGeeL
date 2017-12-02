#define GLEW_STATIC
#include <glew.h>
#include <vector>
#include "texturing/textureparams.h"
#include "texturing/texture.h"
#include "shader.h"
#include "bindingstack.h"
#include <iostream>

namespace geeL {

	std::vector<const TextureBinding*> bindings(TextureBindingStack::MAX_TEXTURE_BINDINGS, nullptr);


	void TextureBindingStack::bindTexturesSimple(const Shader& shader, unsigned int offset) {
		shader.iterateTextures([&offset, &shader](const TextureBinding& binding) {
			binding.texture->bind(offset + binding.offset);
		});
	}

	void TextureBindingStack::bindTexturesDynamic(const Shader& shader, unsigned int offset) {
		std::vector<const TextureBinding*> tempBindings(TextureBindingStack::MAX_TEXTURE_BINDINGS, nullptr);

		unsigned int maxPos = 0;
		shader.iterateTextures([&maxPos, &offset, &shader, &tempBindings](const TextureBinding& binding) {
			unsigned int o = offset + binding.offset;

			if (bindings[o] == nullptr || *bindings[o] != binding) {
				binding.texture->bind(o);

			}

			tempBindings[o] = &binding;
			maxPos = (binding.offset > maxPos) ? binding.offset : maxPos;
		});

		maxPos += offset + 1;
		clearUnits(maxPos);

		bindings.clear();
		bindings = tempBindings;
	}

	void TextureBindingStack::bindSingleTexture(unsigned int ID, const Shader& shader, 
		unsigned int offset, const std::string& name, TextureType type) {

		glActiveTexture(GL_TEXTURE0 + offset);

		TextureParameters::unbind(offset);
		shader.bind<int>(name, offset);
		glBindTexture((int)type, ID);

	}

	void TextureBindingStack::clearUnits(unsigned int start, unsigned int end) {

		int layer = GL_TEXTURE0;
		for (unsigned int i = start; i <= end; i++) {
			const TextureBinding* binding = bindings[i];

			if (binding != nullptr) {
				const ITexture& texture = *binding->texture;

				glActiveTexture(layer + i);
				texture.unbind();
				texture.disable();
			}
		}
	}

}