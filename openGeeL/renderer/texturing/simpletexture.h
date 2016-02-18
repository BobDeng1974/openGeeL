#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include "texture.h"

namespace geeL {

class SimpleTexture : Texture {

public:
	const char* name;

	SimpleTexture() {}
	SimpleTexture(const char* name, const char* fileName, GLint wrapMode = GL_REPEAT, GLint filterMode = GL_LINEAR);
	~SimpleTexture();

	virtual void bind(const Shader* shader, int texLayer = 0);
	virtual void draw(int texLayer = 0);

private:
	GLuint texture;

};

}

#endif