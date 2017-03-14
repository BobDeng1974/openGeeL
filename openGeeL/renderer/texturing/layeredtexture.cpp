#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "../shader/shader.h"
#include "simpletexture.h"
#include "layeredtexture.h"

using namespace std;

namespace geeL {

	void LayeredTexture::addTexture(string name, TextureMap& texture) {
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
			case MapType::Reflection:
				mapFlags += 10000;
				break;
		}

		textures.push_back(pair<string, TextureMap*>(name, &texture));
	}

	void LayeredTexture::bind(const Shader& shader, std::string name, int texLayer) const {

		int counter = 0;
		for_each(textures.begin(), textures.end(), [&](pair<std::string, TextureMap*> tex) {
			TextureMap* texture = tex.second;

			texture->bind(shader, tex.first, counter + shader.mapBindingPos);
			counter++;
		});
	}

	void LayeredTexture::draw(const Shader& shader, int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		int counter = 0;
		for_each(textures.begin(), textures.end(), [&](pair<std::string, TextureMap*> tex) {
			TextureMap* texture = tex.second;

			glActiveTexture(layer + counter + shader.mapBindingPos);
			texture->draw(shader);
			counter++;
		});
	}

	unsigned int LayeredTexture::getID() const {
		return 0;
	}

}