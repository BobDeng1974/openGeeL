#define GLEW_STATIC
#include <glew.h>
#include <vector>
#include "texturing/textureparams.h"
#include "texturing/texture.h"
#include "shader.h"
#include "bindingstack.h"
#include <iostream>

namespace geeL {

	StackPosition stackPosition = 0;
	std::vector<TextureID> bindings(TextureBindingStack::MAX_TEXTURE_ACTIVE, 0);
	std::map<TextureID, StackPosition> positions;


	void addTexture(TextureID id, StackPosition position) {
		bindings[position] = id;
		positions[id] = position;
	}

	void TextureBindingStack::bindTextures(const Shader& shader, unsigned int offset) {

		StackPosition counter = 0;
		shader.iterateTextures([&offset, &shader, &counter](const TextureBinding& binding) {
			TextureID& b = bindings[offset + binding.offset];
			const ITexture& texture = *binding.texture;

			if (shader.ignoreOptimisations || b != texture.getID()) {
				texture.bind(offset + binding.offset);
				addTexture(texture.getID(), offset + binding.offset);
			}

			counter++;
		});

		stackPosition = counter;
	}

	void TextureBindingStack::bindSingleTexture(unsigned int GID, TextureID ID, const Shader& shader,
		unsigned int offset, const std::string& name, TextureType type) {

		glActiveTexture(GL_TEXTURE0 + offset);

		TextureParameters::unbind(offset);
		shader.bind<int>(name, offset);
		glBindTexture((int)type, GID);

		bindings[offset] = ID;
	}

	StackPosition TextureBindingStack::activateTexture(const ITexture& texture) {
		addTexture(texture.getID(), stackPosition);

		glActiveTexture(GL_TEXTURE0 + stackPosition);
		texture.bind(stackPosition);

		return stackPosition++;
	}


	void TextureBindingStack::unbindTexture(const ITexture& texture) {
		auto it(positions.find(texture.getID()));
		if (it != positions.end()) {
			StackPosition position = it->second;

			bindings[position] = 0;
			positions.erase(it);

			glActiveTexture(GL_TEXTURE0 + position);
			texture.unbind();
		}
	}

}