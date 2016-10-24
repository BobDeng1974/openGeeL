#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <string>
#include "../shader/shader.h"

using namespace std;

typedef unsigned int GLuint;

#include "../renderer.h"

namespace geeL {

class Shader;
class PostProcessingScreen;

class PostProcessingEffect : Drawer {

public:
	PostProcessingEffect(string shaderPath);

	//Set buffer that will be used as base for post processing
	void setBuffer(unsigned int buffer);
	virtual void setScreen(PostProcessingScreen& screen);

	void draw();

protected:
	unsigned int buffer;
	Shader shader;
	PostProcessingScreen* screen;

	virtual void bindValues() = 0;
	void bindToScreen();


};




}

#endif
