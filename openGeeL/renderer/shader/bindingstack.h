#ifndef TEXTUREBINDINGSTACK_H
#define TEXTUREBINDINGSTACK_H

#include <functional>

namespace geeL {

	using TextureID = unsigned int;
	using StackPosition = unsigned int;

	class Shader;
	enum class TextureType;


	class TextureBindingStack {

	public:
		static const unsigned int MAX_TEXTURE_BINDINGS = 12;
		static const unsigned int MAX_TEXTURE_ACTIVE = 80;

		static void bindTextures(const Shader& shader, unsigned int offset);
		static void bindSingleTexture(unsigned int GID, TextureID ID, const Shader& shader,
			unsigned int offset, const std::string& name, TextureType type);

		static StackPosition activateTexture(const ITexture& texture);

		static void unbindTexture(const ITexture& texture);

		template<typename... ITextures>
		static void unbindTexture(const ITexture& texture, const ITextures& ...textures);

	};


	template<typename ...ITextures>
	inline void TextureBindingStack::unbindTexture(const ITexture& texture, const ITextures& ...textures) {
		unbindTexture(texture);
		unbindTexture(textures...);
	}

}

#endif
