#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <string>
#include <vector>
#include <vec3.hpp>
#include "maptype.h"
#include "texture.h"
#include "functionaltexture.h"

namespace geeL {

	//Simple 2D texture loaded an from image file
	class ImageTexture : public FunctionalTexture {

	public:
		std::string path;

		ImageTexture(const char* fileName, 
			ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, 
			FilterMode filterMode = FilterMode::None, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

	private:
		struct ImageContainer {
			std::string path;
			int width, height;
			unsigned char* image;

			ImageContainer(const char* fileName);
			~ImageContainer();
		};

		ImageTexture(ImageContainer&& container, 
			ColorType colorType,
			WrapMode wrapMode, 
			FilterMode filterMode,
			AnisotropicFilter filter);

		ImageTexture(const ImageTexture& other) = delete;
		ImageTexture& operator= (const ImageTexture& other) = delete;

	};


	//Texture for mapping purposes (like diffuse, normal mapping) 
	//that can be directly bound to a shader
	class TextureMap : public ImageTexture {

	public:
		MapType type;
		
		TextureMap(const char* fileName, 
			MapType textureTpe = MapType::Diffuse, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		std::string getTypeAsString() const;

	private:
		TextureMap(const TextureMap& other) = delete;
		TextureMap& operator= (const TextureMap& other) = delete;

	};

}

#endif