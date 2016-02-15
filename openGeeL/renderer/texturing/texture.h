#ifndef TEXTURE_H
#define TEXTURE_H

#include <glfw3.h>

namespace geeL {

class Texture {

public:

	virtual void draw(GLint shader, int texLayer = 0) = 0;
};

}

#endif
