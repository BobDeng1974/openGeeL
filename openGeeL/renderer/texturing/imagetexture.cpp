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
			: path(fileName)
			, textureType(TextureType::Texture2D) {

		container = new ImageContainer(fileName, colorType, filterMode, wrapMode, filter);
	}

	ImageTexture::ImageTexture(ImageData image,
		int width,
		int height,
		ColorType colorType,
		FilterMode filterMode,
		WrapMode wrapMode,
		AnisotropicFilter filter)	
			: path("No path specified")
			, textureType(TextureType::Texture2D) {

		container = new ImageContainer(std::move(image), width, height, colorType, 
			filterMode, wrapMode, filter);
	}

	ImageTexture::~ImageTexture() {
		if (container != nullptr)
			delete container;
	}


	void ImageTexture::initGL() {
		ImageContainer& c = *container;

		Texture2D* newTexture = new Texture2D(Resolution(c.width, c.height),
			c.colorType, c.filterMode, c.wrapMode, c.aFilter, c.data.image);
		newTexture->mipmap();

		updateTexture(std::unique_ptr<Texture>(newTexture));

		delete container;
		container = nullptr;
	}

	void ImageTexture::clearGL() {
		deleteTexture();
	}

	TextureType ImageTexture::getTextureType() const {
		return textureType;
	}

	std::string ImageTexture::toString() const {
		return path;
	}

	void ImageTexture::setWrapMode(WrapMode mode) {
		if (container != nullptr)
			container->wrapMode = mode;
	}

	
	
	ImageTexture::ImageData::ImageData(void* image)
		: image(image) {}

	ImageTexture::ImageData::ImageData(ImageData&& data)
		: image(data.image) {

		data.image = nullptr;
	}

	ImageTexture::ImageData::~ImageData() {
		if(image != nullptr)
			stbi_image_free(image);
	}

	ImageTexture::ImageContainer::ImageContainer(const char* fileName, 
		ColorType colorType, 
		FilterMode filterMode, 
		WrapMode wrapMode, 
		AnisotropicFilter aFilter)
			: colorType(colorType)
			, filterMode(filterMode)
			, wrapMode(wrapMode)
			, aFilter(aFilter) 
			, data(stbi_load(fileName, &width, &height, 0, STBI_rgb_alpha)) {}

	ImageTexture::ImageContainer::ImageContainer(ImageData image, int width, int height,
		ColorType colorType, 
		FilterMode filterMode, 
		WrapMode wrapMode, 
		AnisotropicFilter aFilter) 
			: data(std::move(image))
			, width(width)
			, height(height)
			, colorType(colorType)
			, filterMode(filterMode)
			, wrapMode(wrapMode)
			, aFilter(aFilter) {}



	TextureMap::TextureMap(const char* fileName, MapType type, ColorType colorType,
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter)
			: ImageTexture(fileName, colorType, wrapMode, filterMode, filter), type(type) {}


	string TextureMap::getTypeAsString() const {
		return MapTypeConversion::getTypeAsString(type);
	}

}