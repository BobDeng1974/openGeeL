#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <SOIL.h>
#include "simpletexture.h"
#include "../shader/shader.h"

namespace geeL {

	SimpleTexture::SimpleTexture(const char* name, const char* fileName, GLint wrapMode, GLint filterMode) : name(name) {

		int imgWidth, imgHeight;
		unsigned char* image = SOIL_load_image(fileName, &imgWidth, &imgHeight, 0, SOIL_LOAD_RGB);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	SimpleTexture::~SimpleTexture() {
		//delete name;
	}

	void SimpleTexture::bind(const Shader& shader, int texLayer) {
		glUniform1i(glGetUniformLocation(shader.program, name), texLayer);
	}

	void SimpleTexture::draw(int texLayer) {
		glBindTexture(GL_TEXTURE_2D, texture);
	}


}