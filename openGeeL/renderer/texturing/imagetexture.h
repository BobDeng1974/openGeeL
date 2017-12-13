#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <iostream>
#include <string>
#include <vector>
#include <vec3.hpp>
#include "memory/memoryobject.h"
#include "renderer/glstructures.h"
#include "maptype.h"
#include "texture.h"
#include "functionaltexture.h"

namespace geeL {

	//Simple 2D texture loaded from an image file
	//Note: This structure is useless before being initialized and after
	//being cleared and should be used with caution in these cases
	class ImageTexture : public FunctionalTexture, public GLStructure {

	public:
		std::string path;

		ImageTexture(const char* fileName, 
			ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, 
			FilterMode filterMode = FilterMode::None, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);
		virtual ~ImageTexture();

		
		virtual void initGL();
		virtual void clearGL();

		virtual TextureType getTextureType() const;
		virtual std::string toString() const;

		void setWrapMode(WrapMode mode);

	protected:
		//Image data wrapper that will delete data when being destroyed
		struct ImageData {
			void* image;

			explicit ImageData(void* image);
			ImageData(ImageData&& data);
			~ImageData();

		private:
			ImageData(const ImageData& data) = delete;

		};

		TextureType textureType;

		ImageTexture(ImageData image, int width, int height,
			ColorType colorType = ColorType::RGBA,
			FilterMode filterMode = FilterMode::None,
			WrapMode wrapMode = WrapMode::Repeat,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

	private:
		struct ImageContainer {
			ColorType colorType;
			WrapMode wrapMode;
			FilterMode filterMode;
			AnisotropicFilter aFilter;

			int width, height;
			ImageData data;

			ImageContainer(const char* fileName, 
				ColorType colorType, FilterMode filterMode, 
				WrapMode wrapMode, AnisotropicFilter aFilter);
			ImageContainer(ImageData image, int width, int height,
				ColorType colorType, FilterMode filterMode,
				WrapMode wrapMode, AnisotropicFilter aFilter);

		};

		ImageContainer* container;

		ImageTexture(const ImageTexture& other) = delete;
		ImageTexture& operator= (const ImageTexture& other) = delete;

	};


	//Texture for mapping purposes (like diffuse, normal mapping) 
	//that can be directly bound to a shader
	class TextureMap : public ImageTexture {

	public:
		MapType type;
		
		TextureMap(const char* fileName, 
			MapType textureTpe = MapType::Diffuse, 
			ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, 
			FilterMode filterMode = FilterMode::None,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		std::string getTypeAsString() const;

	private:
		TextureMap(const TextureMap& other) = delete;
		TextureMap& operator= (const TextureMap& other) = delete;

	};

}

#endif