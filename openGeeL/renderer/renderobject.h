#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "inputmanager.h"
#include "window.h"

namespace geeL {

class RenderObject {

public:
	const RenderWindow* window;
	const InputManager* inputManager;

	virtual void init() = 0;
	virtual void update() = 0;
	virtual void quit() = 0;
};


}


#endif