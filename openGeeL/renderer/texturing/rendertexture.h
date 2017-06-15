#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "texture.h"

namespace geeL {

	//Texture that is intended to be rendered into
	class RenderTexture : public Texture {

	public:
		RenderTexture() {}
		RenderTexture(const RenderTexture& texture);
		RenderTexture(unsigned int width, unsigned int height, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None);
		
		virtual unsigned int getID() const;
		virtual void remove();

	private:
		unsigned int id, width, height;

	};

}

#endif