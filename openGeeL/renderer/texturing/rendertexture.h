#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "framebuffer/rendertarget.h"
#include "texture.h"

namespace geeL {

	//Texture that is intended to be rendered into
	class RenderTexture : public Texture2D, public RenderTarget {

	public:
		RenderTexture();
		RenderTexture(Resolution resolution, 
			ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, 
			FilterMode filterMode = FilterMode::None);
		RenderTexture(const Texture& other, Resolution resolution);

		virtual void setRenderResolution() const;
		virtual const Resolution& getResolution() const;
		virtual unsigned int getSize() const;

		virtual void resize(Resolution resolution);

	protected:
		virtual void assignInner(unsigned int position) const;

	private:
		RenderTexture(const RenderTexture& other) = delete;
		RenderTexture& operator= (const RenderTexture& other) = delete;

	};

}

#endif