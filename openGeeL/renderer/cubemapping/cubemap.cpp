#define GLEW_STATIC
#include <glew.h>
#include <SOIL.h>
#include "../shader/shader.h"
#include "cubemap.h"

namespace geeL {

	CubeMap::CubeMap(string rightPath, string leftPath, string topPath,
		string bottomPath, string backPath, string frontPath) {

		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);

		int width, height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		image = SOIL_load_image(rightPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = SOIL_load_image(leftPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = SOIL_load_image(topPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = SOIL_load_image(bottomPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = SOIL_load_image(backPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = SOIL_load_image(frontPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}


	void CubeMap::draw(const Shader& shader, string name) {

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader.program, name.c_str()), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	}

}