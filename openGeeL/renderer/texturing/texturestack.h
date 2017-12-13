#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <functional>
#include <utility>
#include <map>
#include "memory/memoryobject.h"
#include "texture.h"
#include "imagetexture.h"

namespace geeL {

	class RenderShader;


	class TextureStack {

	public:
		int mapFlags;
		TextureStack() {}

		void addTexture(const std::string& name, memory::MemoryObject<TextureMap> texture);
		void addTexture(const std::string& name, memory::MemoryObject<ITexture> texture, MapType type);

		void bind(const RenderShader& shader) const;
		void draw(const RenderShader& shader) const;

		void iterTextures(std::function<void(const std::string&, const ITexture&)> function) const;

	private:
		std::map<MapType, std::pair<std::string, memory::MemoryObject<ITexture>>> textures;

		void updateMapFlags(MapType type);
		void iterTextures(std::function<void(const std::string&, ITexture&)> function);

	};
}

#endif
