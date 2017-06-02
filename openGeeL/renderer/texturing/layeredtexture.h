#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <utility>
#include <list>
#include "texture.h"
#include "imagetexture.h"

namespace geeL {

	class LayeredTexture : public Texture {

	public:
		int mapFlags;
		LayeredTexture() : mapFlags(0) {}

		//Add texture. Ill behaviour possible (and not checked)
		//if more than one texture of each type is added (e.g. two normal maps)
		void addTexture(std::string name, TextureMap& texture);

		virtual void bind(const RenderShader& shader, std::string name, int texLayer = 0) const;
		virtual void draw(const RenderShader& shader, int texLayer = 0) const;

		virtual unsigned int getID() const;
		virtual void remove();

	private:
		std::list<std::pair<std::string, TextureMap*>> textures;

	};
}

#endif
