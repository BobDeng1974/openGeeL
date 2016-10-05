#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <string>
#include "texture.h"

using namespace std;

typedef int GLint;
typedef unsigned int GLuint;
#define GL_REPEAT 0x2901 
#define GL_LINEAR 0x2601

namespace geeL {

enum TextureType {
	Diffuse,
	Specular,
	Reflection
};

class SimpleTexture : public Texture {

public:
	TextureType type;
	string path;

	SimpleTexture() {}
	SimpleTexture(const char* fileName, bool linear = false, 
		TextureType type = Diffuse, GLint wrapMode = GL_REPEAT, GLint filterMode = GL_LINEAR);

	virtual void bind(const Shader& shader, const char* name, int texLayer = 0) const;
	virtual void draw(int texLayer = 0) const;

	string GetTypeAsString() const;
	const int GetID() const;

private:
	GLuint id;

};

}

#endif