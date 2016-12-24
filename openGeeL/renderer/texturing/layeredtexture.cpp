#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <algorithm>
#include "../shader/shader.h"
#include "simpletexture.h"
#include "layeredtexture.h"

using namespace std;

namespace geeL {

	void LayeredTexture::addTexture(string name, SimpleTexture& texture) {
		switch (texture.type) {
			case Diffuse:
				mapFlags += 1;
				break;
			case Specular:
				mapFlags += 10;
				break;
			case Normal:
				mapFlags += 100;
				break;
			case Metallic:
				mapFlags += 1000;
				break;
			case Reflection:
				mapFlags += 10000;
				break;
		}

		textures.push_back(pair<string, SimpleTexture*>(name, &texture));
	}

	void LayeredTexture::bind(const Shader& shader, std::string name, int texLayer) const {

		int counter = 0;
		for_each(textures.begin(), textures.end(), [&](pair<std::string, SimpleTexture*> tex) {
			SimpleTexture* texture = tex.second;

			texture->bind(shader, tex.first, counter + shader.mapBindingPos);
			counter++;
		});
	}

	void LayeredTexture::draw(const Shader& shader, int texLayer) const {
		int layer = GL_TEXTURE0 + texLayer;

		int counter = 0;
		for_each(textures.begin(), textures.end(), [&](pair<std::string, SimpleTexture*> tex) {
			SimpleTexture* texture = tex.second;

			glActiveTexture(layer + counter + shader.mapBindingPos);
			texture->draw(shader);
			counter++;
		});
	}

}