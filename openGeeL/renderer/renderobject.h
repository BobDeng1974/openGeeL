#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "inputmanager.h"
#include "window.h"

namespace geeL {

class Camera;

class RenderObject {

public:
	RenderObject() {}

	virtual void init(const Camera* const camera) = 0;
	virtual void draw(const Camera* const camera) = 0;
	virtual void handleInput(const InputManager& input) {}
	virtual void quit() = 0;
};


}


#endif