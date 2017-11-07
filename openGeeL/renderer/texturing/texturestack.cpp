#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "shader/rendershader.h"
#include "imagetexture.h"
#include "texturestack.h"
#include <iostream>

using namespace std;

namespace geeL {

	void TextureStack::addTexture(const string& name, TextureMap& texture) {
		addTexture(name, texture, texture.type);
	}

	void TextureStack::addTexture(const std::string& name, Texture2D& texture, MapType type) {
		if (textures.count(type) == 0)
			updateMapFlags(type);

		textures[type] = pair<string, Texture2D*>(name + MapTypeConversion::getTypeAsString(type), &texture);
	}


	void TextureStack::bind(const RenderShader& shader) const {
		int counter = 0;
		iterTextures([&counter, &shader](const std::string& name, const Texture2D& texture) {
			shader.bind<int>(name, counter + shader.mapBindingPos);
			counter++;
		});
	}

	void TextureStack::draw(const RenderShader& shader) const {
		int layer = GL_TEXTURE0;

		int counter = 0;
		iterTextures([&layer, &counter, &shader](const std::string& name, const Texture2D& texture) {
			glActiveTexture(layer + counter + shader.mapBindingPos);
			texture.bind();
			counter++;
		});
	}


	void TextureStack::iterTextures(std::function<void(const std::string&, const Texture2D&)> function) const {
		for (auto it(textures.begin()); it != textures.end(); it++)
			function(it->second.first, *it->second.second);
	}

	void TextureStack::iterTextures(std::function<void(const std::string&, Texture2D&)> function) {
		for (auto it(textures.begin()); it != textures.end(); it++)
			function(it->second.first, *it->second.second);
	}


	void TextureStack::updateMapFlags(MapType type) {
		switch (type) {
			case MapType::Diffuse:
				mapFlags += 1;
				break;
			case MapType::Specular:
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
		}
	}

}