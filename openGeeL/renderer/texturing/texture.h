#ifndef TEXTURE_H
#define TEXTURE_H

namespace geeL {

class Shader;

class Texture {

public:

	virtual void bind(const Shader& shader, const char* name, int texLayer = 0) const = 0;
	virtual void draw(int texLayer = 0) const = 0;
};

}

#endif
