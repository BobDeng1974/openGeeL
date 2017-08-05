#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "shader/rendershader.h"
#include "imagetexture.h"
#include "layeredtexture.h"

using namespace std;

namespace geeL {

	void LayeredTexture::addTexture(const string& name, TextureMap& texture) {
		addTexture(name, texture, texture.type);
	}

	void LayeredTexture::addTexture(const std::string& name, Texture2D& texture, MapType type) {
		//Set color type to the one of the first added map
		if (textures.size() == 0)
			colorType = texture.getColorType();

		if (textures.count(type) == 0)
			updateMapFlags(type);

		textures[type] = pair<string, Texture2D*>(name + MapTypeConversion::getTypeAsString(type), &texture);
	}


	void LayeredTexture::bind() const {
		auto it = textures.begin();
		Texture* map = it->second.second;

		if (map != nullptr)
			return map->bind();
	}

	void LayeredTexture::bind(const RenderShader& shader, int texLayer) const {
		int counter = 0;
		iterTextures([&counter, &shader](const std::string& name, const Texture2D& texture) {
			shader.bind<int>(name, counter + shader.mapBindingPos);
			counter++;
		});
	}

	void LayeredTexture::draw(const RenderShader& shader, int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		int counter = 0;
		iterTextures([&layer, &counter, &shader](const std::string& name, const Texture2D& texture) {
			glActiveTexture(layer + counter + shader.mapBindingPos);
			texture.bind();
			counter++;
		});
	}

	unsigned int LayeredTexture::getID() const {
		auto it = textures.begin();
		Texture2D* map = it->second.second;

		if (map != nullptr)
			return map->getID();

		return 0;
	}

	void LayeredTexture::remove() {
		iterTextures([](const std::string& name, Texture2D& texture) {
			texture.remove();
		});
	}

	void LayeredTexture::iterTextures(std::function<void(const std::string&, const Texture2D&)> function) const {
		for (auto it(textures.begin()); it != textures.end(); it++)
			function(it->second.first, *it->second.second);
	}

	void LayeredTexture::iterTextures(std::function<void(const std::string&, Texture2D&)> function) {
		for (auto it(textures.begin()); it != textures.end(); it++)
			function(it->second.first, *it->second.second);
	}


	void LayeredTexture::updateMapFlags(MapType type) {
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
		}
	}
}