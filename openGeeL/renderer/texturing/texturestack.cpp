#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "shader/rendershader.h"
#include "imagetexture.h"
#include "texturestack.h"
#include <iostream>

using namespace std;
using namespace geeL::memory;

namespace geeL {

	void TextureStack::addTexture(const string& name, MemoryObject<TextureMap> texture) {
		addTexture(name, texture, texture->type);
	}

	void TextureStack::addTexture(const std::string& name, MemoryObject<ITexture> texture, MapType type) {
		if (textures.count(type) == 0)
			updateMapFlags(type);

		textures[type] = pair<string, MemoryObject<ITexture>>(name + MapTypeConversion::getTypeAsString(type), texture);
	}


	void TextureStack::bindTextures(RenderShader& shader) const {
		iterTextures([&shader](const std::string& name, const ITexture& texture) {
			shader.addMap(texture, name);
		});
	}


	void TextureStack::iterTextures(std::function<void(const std::string&, const ITexture&)> function) const {
		for (auto it(textures.begin()); it != textures.end(); it++)
			function(it->second.first, *it->second.second);
	}

	void TextureStack::iterTextures(std::function<void(const std::string&, ITexture&)> function) {
		for (auto it(textures.begin()); it != textures.end(); it++)
			function(it->second.first, *it->second.second);
	}


	void TextureStack::updateMapFlags(MapType type) {
		switch (type) {
			case MapType::Diffuse:
				mapFlags += 1;
				break;
			case MapType::Gloss:
				mapFlags += 10;
				break;
			case MapType::Normal:
				mapFlags += 100;
				break;
			case MapType::Metallic:
				mapFlags += 1000;
				break;
			case MapType::Alpha:
				mapFlags += 10000;
				break;
			case MapType::Emission:
				mapFlags += 100000;
				break;
			case MapType::Occlusion:
				mapFlags += 1000000;
				break;
			case MapType::Translucency:
				mapFlags += 10000000;
		}
	}

}