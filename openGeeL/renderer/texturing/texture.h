#ifndef TEXTURE_H
#define TEXTURE_H

namespace geeL {

class Shader;

class Texture {

public:

	virtual void bind(const Shader& shader, int texLayer = 0) = 0;
	virtual void draw(int texLayer = 0) = 0;
};

}

#endif
