#define GLEW_STATIC
#include <glew.h>
#include <vector>
#include "texturing/texture.h"
#include "shader.h"
#include "bindingstack.h"
#include <iostream>

namespace geeL {

	std::vector<const TextureBinding*> bindings(TextureBindingStack::MAX_TEXTURE_BINDINGS, nullptr);


	void TextureBindingStack::bindTexturesSimple(const Shader& shader, unsigned int offset) {
		int layer = GL_TEXTURE0;
		shader.iterateTextures([&layer, &offset, &shader](const TextureBinding& binding) {
			binding.texture->bind(layer + offset + binding.offset);
		});
	}

	void TextureBindingStack::bindTexturesDynamic(const Shader & shader, unsigned int offset) {
		std::vector<const TextureBinding*> tempBindings(TextureBindingStack::MAX_TEXTURE_BINDINGS, nullptr);

		int layer = GL_TEXTURE0;
		unsigned int maxPos = 0;
		shader.iterateTextures([&layer, &maxPos, &offset, &shader, &tempBindings](const TextureBinding& binding) {
			unsigned int o = offset + binding.offset;

			if (bindings[o] == nullptr || *bindings[o] != binding) {
				binding.texture->bind(layer + o);

			}

			tempBindings[o] = &binding;
			maxPos = (binding.offset > maxPos) ? binding.offset : maxPos;
		});

		maxPos += offset + 1;
		clearUnits(maxPos);

		bindings.clear();
		bindings = tempBindings;
	}

	void TextureBindingStack::clearUnits(unsigned int start, unsigned int end) {

		int layer = GL_TEXTURE0;
		for (int i = start; i <= end; i++) {
			const TextureBinding* binding = bindings[i];

			if (binding != nullptr) {
				const Texture& texture = *binding->texture;

				glActiveTexture(layer + i);
				texture.unbind();
				texture.disable();
			}
		}
	}

}