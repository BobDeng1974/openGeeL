#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "stb_image.h"
#include "imagetexture.h"
#include "shader/rendershader.h"
#include <iostream>

using namespace std;

namespace geeL {

	ImageTexture::ImageTexture(const char* fileName, ColorType colorType, WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter)
		: Texture2D(colorType), path(fileName) {
		
		int imgWidth, imgHeight;
		unsigned char* image = stbi_load(fileName, &imgWidth, &imgHeight, 0, STBI_rgb_alpha);

		setResolution(imgWidth, imgHeight);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		initColorType(imgWidth, imgHeight, image);
		initWrapMode(wrapMode);
		initFilterMode(filterMode);
		initAnisotropyFilter(filter);

		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	ImageTexture::~ImageTexture() {
		remove();
	}

	ImageTexture::ImageTexture(std::vector<glm::vec3>& colors, unsigned int width, unsigned int height,
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter) : Texture2D(colorType) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, &colors[0]);

		Texture2D::initFilterMode(filterMode);
		Texture2D::initWrapMode(wrapMode);
		
		initAnisotropyFilter(filter);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int ImageTexture::getID() const {
		return id;
	}

	void ImageTexture::remove() {
		glDeleteTextures(1, &id);
	}


	TextureMap::TextureMap(const char* fileName, MapType type, ColorType colorType, 
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter)
			: ImageTexture(fileName, colorType, wrapMode, filterMode, filter), type(type) {}


	string TextureMap::getTypeAsString() const {
		return MapTypeConversion::getTypeAsString(type);
	}

}