#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <vector>
#include <string>
#include <vec3.hpp>
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
	Normal,
	Reflection
};

enum ColorType {
	ColorSingle,
	ColorRGB,
	ColorRGBA,
};

class SimpleTexture : public Texture {

public:
	TextureType type;
	string path;

	SimpleTexture() {}
	SimpleTexture(const char* fileName, bool linear = false, 
		TextureType textureTpe = Diffuse, ColorType colorType = ColorRGBA, 
		int wrapMode = GL_REPEAT, int filterMode = GL_LINEAR);

	SimpleTexture(std::vector<glm::vec3>& colors, unsigned int width, unsigned int height, 
		int wrapMode = GL_REPEAT, int filterMode = GL_LINEAR);

	virtual void bind(const Shader& shader, const char* name, int texLayer = 0) const;
	virtual void draw(const Shader& shader, int texLayer = 0) const;

	string GetTypeAsString() const;
	const int GetID() const;

private:
	GLuint id;

};




}

#endif