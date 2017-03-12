#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "stb_image.h"
#include "simpletexture.h"
#include "../shader/shader.h"
#include <iostream>

using namespace std;

namespace geeL {

	SimpleTexture::SimpleTexture(const char* fileName, bool linear, ColorType colorType, int wrapMode, FilterMode filterMode)
		:  path(fileName) {
		
		int imgWidth, imgHeight;

		unsigned char* image = stbi_load(fileName, &imgWidth, &imgHeight, 0, STBI_rgb_alpha);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		if (linear) {
			switch (colorType) {
				case ColorSingle:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
					break;
				case ColorRGB:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
					break;
				case ColorRGBA:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
					break;
			}
		}
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		switch (filterMode) {
			case None:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;
			case Linear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case Bilinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
			case Trilinear:
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;
		}

		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	SimpleTexture::SimpleTexture(std::vector<glm::vec3>& colors, unsigned int width, unsigned int height,
		int wrapMode, int filterMode) {

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, &colors[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int SimpleTexture::getID() const {
		return id;
	}



	TextureMap::TextureMap(const char* fileName, bool linear, TextureType type, ColorType colorType, int wrapMode, FilterMode filterMode)
		: SimpleTexture(fileName, linear, colorType, wrapMode, filterMode), type(type) {}


	void TextureMap::bind(const Shader& shader, std::string name, int texLayer) const {
		glUniform1i(glGetUniformLocation(shader.program, name.c_str()), texLayer);
	}

	void TextureMap::draw(const Shader& shader, int texLayer) const {
		glBindTexture(GL_TEXTURE_2D, id);
	}

	string TextureMap::getTypeAsString() const {
		switch (type) {
			case Diffuse:
				return "diffuse";
			case Specular:
				return "specular";
			case Normal:
				return "normal";
			case Reflection:
				return "reflection";
		}

		return "";
	}

}