#ifndef TEXTUREBINDINGSTACK_H
#define TEXTUREBINDINGSTACK_H

#include <functional>

namespace geeL {

	class ITexture;
	class Shader;


	class TextureBindingStack {

	public:
		static const unsigned int MAX_TEXTURE_BINDINGS = 12;

		static void bindTextures(const Shader& shader, unsigned int offset);
		static void bindSingleTexture(unsigned int GID, unsigned int ID, const Shader& shader,
			unsigned int offset, const std::string& name, TextureType type);

	};

}

#endif
