#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <SOIL.h>
#include "simpletexture.h"
#include "../shader/shader.h"

namespace geeL {

	SimpleTexture::SimpleTexture(const char* fileName, TextureType type, GLint wrapMode, GLint filterMode) 
		: type(type), path(fileName) {

		int imgWidth, imgHeight;
		unsigned char* image = SOIL_load_image(fileName, &imgWidth, &imgHeight, 0, SOIL_LOAD_RGBA);

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SimpleTexture::bind(const Shader& shader, const char* name, int texLayer) const {
		glUniform1i(glGetUniformLocation(shader.program, name), texLayer);
	}

	void SimpleTexture::draw(int texLayer) const {
		glBindTexture(GL_TEXTURE_2D, id);
	}

	string SimpleTexture::GetTypeAsString() const {
		switch (type) {
			case Diffuse:
				return "diffuse";
			case Specular:
				return "specular";
		}

		return "";
	}

	const int SimpleTexture::GetID() const {
		return id;
	}

}