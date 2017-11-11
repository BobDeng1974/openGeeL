#ifndef TEXTUREBINDINGSTACK_H
#define TEXTUREBINDINGSTACK_H

#include <functional>

namespace geeL {

	class Shader;


	class TextureBindingStack {

	public:
		static const unsigned int MAX_TEXTURE_BINDINGS = 10;

		static void bindTexturesSimple(const Shader& shader, unsigned int offset);
		static void bindTexturesDynamic(const Shader& shader, unsigned int offset);
		static void bindSingleTexture(unsigned int ID, const Shader& shader, unsigned int offset, 
			const std::string& name, TextureType type);

	private:
		static void clearUnits(unsigned int start = 0, unsigned int end = MAX_TEXTURE_BINDINGS);

	};

}

#endif
