#ifndef TEXTUREDCUBEMAP_H
#define TEXTUREDCUBEMAP_H

#include "../texturing/imagetexturecube.h"
#include "cubemap.h"

namespace geeL {

	class ImageTextureCube;
	class RenderShader;

	//Cubemap that consists of six textures for each side
	class TexturedCubeMap : public CubeMap {

	public:
		TexturedCubeMap(ImageTextureCube& texture) : CubeMap(&texture) {}

	};
}

#endif
