#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "texture.h"

namespace geeL {

	//Texture that is intended to be rendered into
	class RenderTexture : public Texture2D {

	public:
		RenderTexture() : Texture2D(ColorType::None) {}
		RenderTexture(Resolution resolution, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None);
		RenderTexture(const Texture& other, Resolution resolution);

		virtual void resize(Resolution resolution);

	private:
		RenderTexture(const RenderTexture& other) = delete;
		RenderTexture& operator= (const RenderTexture& other) = delete;

	};

}

#endif