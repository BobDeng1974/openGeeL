#ifndef POSTPROCESSCREEN_H
#define POSTPROCESSCREEN_H

namespace geeL {

class FrameBuffer;

//Screen for post processing draw calls
class PostProcessingScreen {

public:
	unsigned int vbo;
	unsigned int vao;
	int width;
	int height;

	PostProcessingScreen(int width, int height) 
		: width(width), height(height) {}

	void init();

};

}


#endif