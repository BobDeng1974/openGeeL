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

	ImageTexture::ImageTexture(ImageContainer&& container,
		ColorType colorType,
		WrapMode wrapMode,
		FilterMode filterMode,
		AnisotropicFilter filter)
			: FunctionalTexture(std::unique_ptr<Texture2D>(
				new Texture2D(Resolution(container.width, container.height),
					colorType, 
					filterMode, 
					wrapMode, 
					filter, 
					container.image)))
			, path(container.path) {

		getTexture().mipmap();
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