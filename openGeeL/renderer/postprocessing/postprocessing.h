#ifndef POSTPROCESSINGEFFECT_H
#define POSTPROCESSINGEFFECT_H

#include <string>

using namespace std;

typedef unsigned int GLuint;

namespace geeL {

class Shader;

class PostProcessingEffect {

public:
	PostProcessingEffect(string shaderPath);

	virtual void draw() = 0;
	void bindToScreen(GLuint screenVAO, GLuint colorBuffer);


protected:
	Shader shader;

};

}

#endif
