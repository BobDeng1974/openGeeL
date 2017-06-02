#ifndef SIMPLETEXTURE_H
#define SIMPLETEXTURE_H

#include <vector>
#include <string>
#include <vec3.hpp>
#include "texture.h"

typedef int GLint;
typedef unsigned int GLuint;
#define GL_REPEAT 0x2901 
#define GL_LINEAR 0x2601

namespace geeL {

	enum class MapType {
		Diffuse,
		Specular,
		Normal,
		Reflection,
		Metallic
	};


	//Simple 2D Texutre loaded from image file
	class ImageTexture : Texture {

	public:
		std::string path;

		ImageTexture() {}
		ImageTexture(const ImageTexture& texture);
		ImageTexture(const char* fileName, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::Nearest, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		ImageTexture(std::vector<glm::vec3>& colors, unsigned int width, unsigned int height,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::Nearest,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		virtual unsigned int getID() const;
		virtual void remove();

	protected:
		unsigned int id;

	};


	//Texture for mapping purposes (like diffuse, normal mapping) that can be directly bound to a shader
	class TextureMap : public ImageTexture {

	public:
		MapType type;
		
		TextureMap() {}
		TextureMap(const TextureMap& map);
		TextureMap(const char* fileName, 
			MapType textureTpe = MapType::Diffuse, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::Nearest,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		virtual void bind(const RenderShader& shader, std::string name, int texLayer = 0) const;
		virtual void draw(const RenderShader& shader, int texLayer = 0) const;

		std::string getTypeAsString() const;
	};

}

#endif