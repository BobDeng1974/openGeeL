#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "framebuffer/rendertarget.h"
#include "texture.h"

namespace geeL {

	//Texture that is intended to be rendered into
	class RenderTexture : public Texture2D, public RenderTarget {

	public:
		RenderTexture() : Texture2D(ColorType::None) {}
		RenderTexture(Resolution resolution, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None);
		RenderTexture(const Texture& other, Resolution resolution);


		//Assing this texture to given framebuffer
		virtual void assignTo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false);

		//Assign this texture to given framebuffer besides an already assigned main buffer. 
		//Note: This function will only work if texture has been assigned to a main buffer beforehand
		virtual bool assignToo(const IFrameBuffer& buffer, unsigned int position, bool bindFB = false) const;

		virtual void setRenderResolution() const;
		virtual const Resolution& getResolution() const;

		virtual void resize(Resolution resolution);

	private:
		RenderTexture(const RenderTexture& other) = delete;
		RenderTexture& operator= (const RenderTexture& other) = delete;

	};

}

#endif