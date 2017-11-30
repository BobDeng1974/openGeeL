#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "stb_image.h"
#include "imagetexture.h"
#include "shader/rendershader.h"

using namespace std;

namespace geeL {

	ImageTexture::ImageTexture(const char* fileName, 
		ColorType colorType, 
		WrapMode wrapMode, 
		FilterMode filterMode, 
		AnisotropicFilter filter)
			: ImageTexture(ImageContainer(fileName), 
				colorType, 
				wrapMode, 
				filterMode, 
				filter) {}

	ImageTexture::ImageTexture(std::vector<glm::vec3>& colors, 
		const Resolution& resolution,
		ColorType colorType,
		WrapMode wrapMode, 
		FilterMode filterMode, 
		AnisotropicFilter filter) 
			: Texture2D(resolution, 
				colorType, 
				filterMode, 
				wrapMode, 
				filter, 
				&colors[0]) {}

	ImageTexture::ImageTexture(ImageContainer&& container,
		ColorType colorType,
		WrapMode wrapMode,
		FilterMode filterMode,
		AnisotropicFilter filter)
			: Texture2D(Resolution(container.width, container.height), 
				colorType, filterMode, 
				wrapMode, filter, container.image)
			, path(container.path) {

		mipmap();
	}


	TextureMap::TextureMap(const char* fileName, MapType type, ColorType colorType, 
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter)
			: ImageTexture(fileName, colorType, wrapMode, filterMode, filter), type(type) {}


	string TextureMap::getTypeAsString() const {
		return MapTypeConversion::getTypeAsString(type);
	}

	ImageTexture::ImageContainer::ImageContainer(const char* fileName)
		: path(fileName) {

		image = stbi_load(fileName, &width, &height, 0, STBI_rgb_alpha);
	}

	ImageTexture::ImageContainer::~ImageContainer() {
		stbi_image_free(image);
	}

}