#ifndef TEXTURETARGET_H
#define TEXTURETARGET_H

#include "framebuffer/rendertarget.h"
#include "functionaltexture.h"

namespace geeL {

	class TextureTarget : public RenderTarget, public FunctionalTexture {

	public:
		TextureTarget(std::unique_ptr<Texture> texture);

		virtual void setRenderResolution() const;
		virtual Resolution getRenderResolution() const;
		virtual unsigned int getSize() const;


		template<typename TextureType, typename ...TextureArgs>
		static TextureTarget createTextureTargetLocal(TextureArgs&& ...args);

		template<typename TextureType, typename ...TextureArgs>
		static std::unique_ptr<TextureTarget> createTextureTarget(TextureArgs&& ...args);


	protected:
		virtual void assign(unsigned int position) const;

	};


	template<typename TextureType, typename ...TextureArgs>
	inline TextureTarget TextureTarget::createTextureTargetLocal(TextureArgs&& ...args) {
		return TextureTarget(std::unique_ptr<TextureType>(
			new TextureType(std::forward<TextureArgs>(args)...)
			));
	}

	template<typename TextureType, typename ...TextureArgs>
	inline std::unique_ptr<TextureTarget> TextureTarget::createTextureTarget(TextureArgs&& ...args) {
		return std::unique_ptr<TextureTarget>(new TextureTarget(
			std::unique_ptr<TextureType>(new TextureType(std::forward<TextureArgs>(args)...)
			)));
	}

}

#endif
