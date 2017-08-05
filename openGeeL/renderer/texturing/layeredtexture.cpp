#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "shader/rendershader.h"
#include "imagetexture.h"
#include "layeredtexture.h"

using namespace std;

namespace geeL {

	void LayeredTexture::addTexture(string name, TextureMap& texture) {
		//Set color type to the one of the first added map
		if (textures.size() == 0)
			colorType = texture.getColorType();

		switch (texture.type) {
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

		textures.push_back(pair<string, TextureMap*>(name, &texture));
	}

	void LayeredTexture::bind() const {
		auto it = textures.begin();
		Texture* map = it->second;

		if (map != nullptr)
			return map->bind();
	}

	void LayeredTexture::bind(const RenderShader& shader, int texLayer) const {

		int counter = 0;
		for_each(textures.begin(), textures.end(), [&](pair<std::string, TextureMap*> tex) {
			TextureMap* texture = tex.second;

			shader.bind<int>(tex.first, counter + shader.mapBindingPos);
			counter++;
		});
	}

	void LayeredTexture::draw(const RenderShader& shader, int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		int counter = 0;
		for_each(textures.begin(), textures.end(), [&](pair<std::string, TextureMap*> tex) {
			Texture2D* texture = tex.second;

			glActiveTexture(layer + counter + shader.mapBindingPos);
			texture->bind();
			counter++;
		});
	}

	unsigned int LayeredTexture::getID() const {
		auto it = textures.begin();
		TextureMap* map = it->second;

		if (map != nullptr)
			return map->getID();

		return 0;
	}

	void LayeredTexture::remove() {
		for_each(textures.begin(), textures.end(), [&](pair<std::string, TextureMap*> tex) {
			TextureMap* texture = tex.second;

			texture->remove();
		});
	}
}