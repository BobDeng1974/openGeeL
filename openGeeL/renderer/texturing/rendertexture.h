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

		virtual void setRenderResolution() const;
		virtual Resolution getRenderResolution() const;
		virtual unsigned int getSize() const;

	protected:
		virtual void assign(unsigned int position) const;

	private:
		RenderTexture(const RenderTexture& other) = delete;
		RenderTexture& operator= (const RenderTexture& other) = delete;

	};

}

#endif