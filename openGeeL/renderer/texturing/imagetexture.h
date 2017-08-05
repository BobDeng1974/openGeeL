#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <vector>
#include <string>
#include <vec3.hpp>
#include "texture.h"

#define GL_REPEAT 0x2901 
#define GL_LINEAR 0x2601

namespace geeL {

	enum class MapType {
		Diffuse,
		Specular,
		Normal,
		Metallic
	};


	//Simple 2D Texutre loaded from image file
	class ImageTexture : public Texture2D {

	public:
		std::string path;

		ImageTexture() : Texture2D(ColorType::None), id(0) {}
		ImageTexture(const char* fileName, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);
		~ImageTexture();

		ImageTexture(std::vector<glm::vec3>& colors, unsigned int width, unsigned int height,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::None,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		virtual unsigned int getID() const;
		virtual void remove();

	protected:
		unsigned int id;

	private:
		ImageTexture(const ImageTexture& other) = delete;
		ImageTexture& operator= (const ImageTexture& other) = delete;

	};


	//Texture for mapping purposes (like diffuse, normal mapping) that can be directly bound to a shader
	class TextureMap : public ImageTexture {

	public:
		MapType type;
		
		TextureMap() {}
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