#ifndef DUMMYTEXTURE_H
#define DUMMYTEXTURE_H

#include "functionaltexture.h"

namespace geeL {

	class DummyTexture {
			
	public:
		//Returns a 1x1 2D texture with no content
		static std::unique_ptr<Texture2D> createTexture2D();

		//Returns a 6x1x1 cubic texture with no content
		static std::unique_ptr<TextureCube> createTextureCube();

	};
}

#endif
