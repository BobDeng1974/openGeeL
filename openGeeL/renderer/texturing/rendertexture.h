#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

#include "framebuffer/rendertarget.h"
#include "functionaltexture.h"
#include "texture.h"

namespace geeL {

	//Texture that is intended to be rendered into
	class RenderTexture : public FunctionalTexture, public RenderTarget {

	public:
		RenderTexture();
		RenderTexture(Resolution resolution, 
			ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, 
			FilterMode filterMode = FilterMode::None);

		virtual void applyRenderResolution() const;

		Resolution getResolution() const;
		virtual Resolution getRenderResolution() const;
		virtual unsigned int getSize() const;
		void resize(Resolution resolution);

		ColorType getColorType() const;
		ResolutionScale getScale() const;


		void attachParameters(const TextureParameters& parameters);
		void detachParameters();
		void mipmap() const;

	protected:
		virtual void assign(unsigned int position) const;

	private:
		RenderTexture(const RenderTexture& other) = delete;
		RenderTexture& operator= (const RenderTexture& other) = delete;

	};

}

#endif