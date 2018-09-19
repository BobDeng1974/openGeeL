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

	private:
		ITextureProvider& provider;

	};

}

#endif
