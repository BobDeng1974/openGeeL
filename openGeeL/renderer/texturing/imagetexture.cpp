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
			: FunctionalTexture()
			, path(fileName) {

		container = new ImageContainer(fileName, colorType, filterMode, wrapMode, filter);
		initGL();
	}

	ImageTexture::~ImageTexture() {
		if (container != nullptr)
			delete container;
	}


	void ImageTexture::initGL() {
		ImageContainer& c = *container;

		Texture2D* newTexture = new Texture2D(Resolution(c.width, c.height),
			c.colorType, c.filterMode, c.wrapMode, c.aFilter, c.image);
		newTexture->mipmap();

		updateTexture(std::unique_ptr<Texture>(newTexture));

		delete container;
		container = nullptr;
	}

	void ImageTexture::clearGL() {
		deleteTexture();
	}
	


	ImageTexture::ImageContainer::ImageContainer(const char* fileName, 
		ColorType colorType, 
		FilterMode filterMode, 
		WrapMode wrapMode, 
		AnisotropicFilter aFilter)
			: colorType(colorType)
			, filterMode(filterMode)
			, wrapMode(wrapMode)
			, aFilter(aFilter) {

		image = stbi_load(fileName, &width, &height, 0, STBI_rgb_alpha);
	}

	ImageTexture::ImageContainer::ImageContainer(void* image, int width, int height, 
		ColorType colorType, 
		FilterMode filterMode, 
		WrapMode wrapMode, 
		AnisotropicFilter aFilter) 
			: image(image)
			, width(width)
			, height(height)
			, colorType(colorType)
			, filterMode(filterMode)
			, wrapMode(wrapMode)
			, aFilter(aFilter) {}

	ImageTexture::ImageContainer::~ImageContainer() {
		stbi_image_free(image);
	}



	TextureMap::TextureMap(const char* fileName, MapType type, ColorType colorType,
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter)
		: ImageTexture(fileName, colorType, wrapMode, filterMode, filter), type(type) {}


	string TextureMap::getTypeAsString() const {
		return MapTypeConversion::getTypeAsString(type);
	}


}