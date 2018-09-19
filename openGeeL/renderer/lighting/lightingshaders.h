#ifndef TRANSPARENTLIGHTING_H
#define TRANSPARENTLIGHTING_H

#include "shader/sceneshader.h"

namespace geeL {

	class ITextureProvider;


	class ForwardLighting : public SceneShader {

	public:
		ForwardLighting(bool animated);

	};

	class HybridLighting : public SceneShader {

	public:
		HybridLighting(bool animated);

	};


	class TransparentLighting : public SceneShader {

	public:
		TransparentLighting(ITextureProvider& provider, bool animated);

		virtual std::string removeMap(const ITexture& texture);
		virtual void removeMap(const std::string& name);

	private:
		ITextureProvider& provider;

		void addTextures(const std::string& name);

	};

}

#endif
