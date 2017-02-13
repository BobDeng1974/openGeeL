#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <utility>
#include <list>
#include "texture.h"
#include "simpletexture.h"

namespace geeL {

	class LayeredTexture : public Texture {

	public:
		int mapFlags;
		LayeredTexture() : mapFlags(0) {}

		//Add texture. Ill behaviour possible (and not checked)
		//if more than one texture of each type is added (e.g. two normal maps)
		void addTexture(std::string name, SimpleTexture& texture);

		virtual void bind(const Shader& shader, std::string name, int texLayer = 0) const;
		virtual void draw(const Shader& shader, int texLayer = 0) const;

	private:
		std::list<std::pair<std::string, SimpleTexture*>> textures;

	};
}

#endif
