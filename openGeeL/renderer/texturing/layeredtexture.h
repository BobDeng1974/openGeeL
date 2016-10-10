#ifndef LAYEREDTEXTURE_H
#define LAYEREDTEXTURE_H

#include <utility>
#include <vector>
#include "texture.h"
#include "simpletexture.h"

using namespace std;

namespace geeL {

//class SimpleTexture;

class LayeredTexture : public Texture {

public:
	LayeredTexture() {}

	void addTexture(string name, SimpleTexture& texture);

	virtual void bind(const Shader& shader, const char* name, int texLayer = 0) const;
	virtual void draw(const Shader& shader, int texLayer = 0) const;

private:
	vector<pair<string, SimpleTexture*>> textures;

};

}

#endif
