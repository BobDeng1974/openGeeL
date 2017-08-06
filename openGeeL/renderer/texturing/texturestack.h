#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <functional>
#include <utility>
#include <map>
#include "texture.h"
#include "imagetexture.h"

namespace geeL {

	class TextureStack : public Texture2D {

	public:
		int mapFlags;
		TextureStack() : Texture2D(ColorType::None), mapFlags(0) {}

		void addTexture(const std::string& name, TextureMap& texture);
		void addTexture(const std::string& name, Texture2D& texture, MapType type);


		//Bind (only) first texture
		virtual void bind() const;
		void bind(const RenderShader& shader) const;
		void draw(const RenderShader& shader) const;

		virtual unsigned int getID() const;
		virtual void remove();

		void iterTextures(std::function<void(const std::string&, const Texture2D&)> function) const;

	private:
		std::map<MapType, std::pair<std::string, Texture2D*>> textures;

		void updateMapFlags(MapType type);
		void iterTextures(std::function<void(const std::string&, Texture2D&)> function);

	};
}

#endif
