#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "stb_image.h"
#include "simpletexture.h"
#include "../shader/shader.h"
#include <iostream>

using namespace std;

namespace geeL {

	SimpleTexture::SimpleTexture(const char* fileName, ColorType colorType, WrapMode wrapMode, FilterMode filterMode)
		:  path(fileName) {
		
		int imgWidth, imgHeight;

		unsigned char* image = stbi_load(fileName, &imgWidth, &imgHeight, 0, STBI_rgb_alpha);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		Texture::initColorType(colorType, imgWidth, imgHeight, image);

		glGenerateMipmap(GL_TEXTURE_2D);

		Texture::initWrapMode(wrapMode);
		Texture::initFilterMode(filterMode);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	SimpleTexture::SimpleTexture(std::vector<glm::vec3>& colors, unsigned int width, unsigned int height,
		WrapMode wrapMode, FilterMode filterMode) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, &colors[0]);

		Texture::initFilterMode(filterMode);
		Texture::initWrapMode(wrapMode),

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int SimpleTexture::getID() const {
		return id;
	}



	TextureMap::TextureMap(const char* fileName, MapType type, ColorType colorType, WrapMode wrapMode, FilterMode filterMode)
		: SimpleTexture(fileName, colorType, wrapMode, filterMode), type(type) {}


	void TextureMap::bind(const Shader& shader, std::string name, int texLayer) const {
		glUniform1i(glGetUniformLocation(shader.program, name.c_str()), texLayer);
	}

	void TextureMap::draw(const Shader& shader, int texLayer) const {
		glBindTexture(GL_TEXTURE_2D, id);
	}

	string TextureMap::getTypeAsString() const {
		switch (type) {
			case MapType::Diffuse:
				return "diffuse";
			case MapType::Specular:
				return "specular";
			case MapType::Normal:
				return "normal";
			case MapType::Reflection:
				return "reflection";
		}

		return "";
	}

}