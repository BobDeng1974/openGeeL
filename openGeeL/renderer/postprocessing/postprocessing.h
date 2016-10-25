#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <string>
#include "../shader/shader.h"

using namespace std;

typedef unsigned int GLuint;

#include <list>
#include "../renderer.h"


namespace geeL {

class Shader;
class PostProcessingScreen;

class PostProcessingEffect : public Drawer {

public:
	PostProcessingEffect(string shaderPath);

	//Set buffer that will be used as base for post processing
	void setBuffer(unsigned int buffer);

	//Set multiple buffers for post processing that will be used in list order. Maximum of 4
	void setBuffer(std::list<unsigned int> buffers);

	virtual void setScreen(PostProcessingScreen& screen);

	void draw();

protected:
	int bindingStart = 1;
	std::list<unsigned int> buffers;
	unsigned int maxBuffers = 4;
	Shader shader;
	PostProcessingScreen* screen;

	virtual void bindValues();
	void bindToScreen();


};




}

#endif
