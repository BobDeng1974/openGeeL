#define GLEW_STATIC
#include <glew.h>
#include "stb_image.h"
#include "imagetexturecube.h"

using namespace std;

namespace geeL {

	ImageTextureCube::ImageTextureCube(const string& rightPath, const string& leftPath, const string& topPath,
		const string& bottomPath, const string& backPath, const string& frontPath, WrapMode wrapMode, FilterMode filterMode) 
			: TextureCube(ColorType::GammaSpace) {

		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);

		int width, height;
		unsigned char* image;

		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		image = stbi_load(rightPath.c_str(), &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = stbi_load(leftPath.c_str(), &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = stbi_load(topPath.c_str(), &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = stbi_load(bottomPath.c_str(), &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = stbi_load(backPath.c_str(), &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		image = stbi_load(frontPath.c_str(), &width, &height, 0, STBI_rgb);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		initFilterMode(filterMode);
		initWrapMode(wrapMode);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		stbi_image_free(image);

		resolution = (width + height) / 2;
	}

	ImageTextureCube::~ImageTextureCube() {
		remove();
	}


	void ImageTextureCube::remove() {
		glDeleteTextures(1, &id);
	}

}