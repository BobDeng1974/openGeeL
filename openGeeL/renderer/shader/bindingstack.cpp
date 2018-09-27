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

	void incrementStackPosition() {
		stackPosition = (stackPosition + 1) % TextureBindingStack::MAX_TEXTURE_BINDINGS;
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

		stackPosition = counter; //Note: Counter can never be higher than max texture binding amount
	}

	void TextureBindingStack::bindSingleTexture(unsigned int GID, TextureID ID, const Shader& shader,
		unsigned int offset, const std::string& name, TextureType type) {

		assert(offset < TextureBindingStack::MAX_TEXTURE_BINDINGS);

		glActiveTexture(GL_TEXTURE0 + offset);

		TextureParameters::unbind(offset);
		shader.bind<int>(name, offset);
		glBindTexture((int)type, GID);

		bindings[offset] = ID;
	}

	StackPosition TextureBindingStack::activateTexture(const ITexture& texture) {
		auto it(positions.find(texture.getID()));
		if (it != positions.end()) return it->second;

		StackPosition position = stackPosition;
		incrementStackPosition();

		addTexture(texture.getID(), position);

		glActiveTexture(GL_TEXTURE0 + position);
		texture.bind(position);

		return position;
	}


	void TextureBindingStack::deactivateTexture(const ITexture& texture) {
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