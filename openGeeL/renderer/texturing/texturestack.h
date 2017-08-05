#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <utility>
#include <map>
#include "texture.h"
#include "imagetexture.h"

namespace geeL {

	//Object that holds a stack of textures of different types (E.g. diffuse or specular maps)
	class TextureStack : public Texture2D {

	public:
		int mapFlags;
		TextureStack() : Texture2D(ColorType::None), mapFlags(0) {}

		//Add texture map
		void addTexture(std::string name, TextureMap& texture);
		void addTexture(std::string name, Texture2D& texture, MapType type);


		//Bind (only) first texture
		virtual void bind() const;
		void bind(const RenderShader& shader, int texLayer = 0) const;
		void draw(const RenderShader& shader, int texLayer = 0) const;

		virtual unsigned int getID() const;
		virtual void remove();

	private:
		std::map<MapType, std::pair<std::string, Texture2D*>> textures;

		void updateMapFlags(MapType type);

	};
}

#endif
