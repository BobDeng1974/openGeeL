#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <string>
#include <vector>
#include <vec3.hpp>
#include "maptype.h"
#include "renderer/glstructures.h"
#include "texture.h"
#include "functionaltexture.h"

namespace geeL {

	//Simple 2D texture loaded from an image file
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

	private:
		struct ImageContainer {
			ColorType colorType;
			WrapMode wrapMode;
			FilterMode filterMode;
			AnisotropicFilter aFilter;

			int width, height;
			void* image;

			ImageContainer(const char* fileName, 
				ColorType colorType, FilterMode filterMode, 
				WrapMode wrapMode, AnisotropicFilter aFilter);
			ImageContainer(void* image, int width, int height,
				ColorType colorType, FilterMode filterMode,
				WrapMode wrapMode, AnisotropicFilter aFilter);

			~ImageContainer();
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