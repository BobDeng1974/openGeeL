#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <list>
#include <string>
#include "../renderer.h"
#include "../shader/shader.h"

using namespace std;

typedef unsigned int GLuint;

namespace geeL {

class Shader;
class ScreenQuad;

class PostProcessingEffect : public Drawer {

public:
	PostProcessingEffect(string fragmentPath);
	PostProcessingEffect(string vertexPath, string fragmentPath);

	//Set (first) buffer that will be used as base for post processing
	void setBuffer(unsigned int buffer);

	//Set multiple buffers for post processing that will be used in list order. Maximum of 4
	void setBuffer(std::list<unsigned int> buffers);

	virtual void setScreen(ScreenQuad& screen);

	virtual void draw();

protected:
	std::list<unsigned int> buffers;
	unsigned int maxBuffers = 4;
	Shader shader;
	ScreenQuad* screen;

	virtual void bindValues();
	virtual void bindToScreen();


};




}

#endif
