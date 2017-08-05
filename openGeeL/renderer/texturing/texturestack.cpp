#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "shader/rendershader.h"
#include "imagetexture.h"
#include "texturestack.h"

using namespace std;

namespace geeL {

	void TextureStack::addTexture(string name, TextureMap& texture) {
		addTexture(name, texture, texture.type);
	}

	void TextureStack::addTexture(std::string name, Texture2D& texture, MapType type) {
		//Set color type to the one of the first added map
		if (textures.size() == 0)
			colorType = texture.getColorType();

		//if (textures.count(type) == 0)
			updateMapFlags(type);

		textures[type] = pair<string, Texture2D*>(name, &texture);
	}


	void TextureStack::bind() const {
		auto it = textures.begin();
		Texture* map = it->second.second;

		if (map != nullptr)
			return map->bind();
	}

	void TextureStack::bind(const RenderShader& shader, int texLayer) const {
		int counter = 0;

		for (auto it(textures.begin()); it != textures.end(); it++) {
			Texture2D* texture = it->second.second;

			shader.bind<int>(it->second.first, counter + shader.mapBindingPos);
			counter++;
		}
	}

	void TextureStack::draw(const RenderShader& shader, int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		int counter = 0;
		for(auto it(textures.begin()); it != textures.end(); it++) {
			Texture2D* texture = it->second.second;
		
			glActiveTexture(layer + counter + shader.mapBindingPos);
			texture->bind();
			counter++;
		}

	}

	unsigned int TextureStack::getID() const {
		auto it = textures.begin();
		Texture2D* map = it->second.second;

		if (map != nullptr)
			return map->getID();

		return 0;
	}

	void TextureStack::remove() {
		for (auto it(textures.begin()); it != textures.end(); it++) {
			Texture2D* texture = it->second.second;
			texture->remove();
		}
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
		}
	}

}