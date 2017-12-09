#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <functional>
#include <utility>
#include <map>
#include <memory>
#include "texture.h"
#include "imagetexture.h"

namespace geeL {

	class RenderShader;


	class TextureStack {

	public:
		int mapFlags;
		TextureStack() {}

		void addTexture(const std::string& name, std::shared_ptr<TextureMap> texture);
		void addTexture(const std::string& name, std::shared_ptr<Texture2D> texture, MapType type);

		void bind(const RenderShader& shader) const;
		void draw(const RenderShader& shader) const;

		void iterTextures(std::function<void(const std::string&, const Texture2D&)> function) const;

	private:
		std::map<MapType, std::pair<std::string, std::shared_ptr<Texture2D>>> textures;

		void updateMapFlags(MapType type);
		void iterTextures(std::function<void(const std::string&, Texture2D&)> function);

	};
}

#endif
