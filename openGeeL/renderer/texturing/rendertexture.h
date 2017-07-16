#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "texture.h"

namespace geeL {

	//Texture that is intended to be rendered into
	class RenderTexture : public Texture2D {

	public:
		RenderTexture() : Texture2D(ColorType::None), id(0) {}
		RenderTexture(Resolution resolution, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None);
		~RenderTexture();

		virtual unsigned int getID() const;
		virtual void remove();

		virtual void resize(Resolution resolution);

	private:
		unsigned int id;

		RenderTexture(const RenderTexture& other) = delete;
		RenderTexture& operator= (const RenderTexture& other) = delete;

	};

}

#endif