#ifndef GBUFFER_H
#define GBUFFER_H

namespace geeL {

class Drawer;

class GBuffer {

public:
	unsigned int fbo;
	unsigned int diffuseSpec;
	unsigned int position;
	unsigned int normal;
	unsigned int depth;
	int width;
	int height;

	GBuffer();
	~GBuffer();

	void init(int width, int height);
	void fill(Drawer& drawer);

private:
	unsigned int generateTexture(unsigned int position, unsigned int type);

};

}

#endif