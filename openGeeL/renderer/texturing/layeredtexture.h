#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <vector>
#include "texture.h"

namespace geeL {

class SimpleTexture;

class LayeredTexture : Texture {

public:
	LayeredTexture() {}

	void addTexture(const SimpleTexture& texture);
	virtual void draw(GLint shader, int texLayer = 0);

private:
	std::vector<SimpleTexture> textures;

};

}

#endif
