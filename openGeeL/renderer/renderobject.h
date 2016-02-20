#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "inputmanager.h"
#include "window.h"
#include "shader/shadermanager.h"

namespace geeL {

class Camera;

class RenderObject {

public:
	const RenderWindow* const window;
	ShaderManager* const shaderManager;

	RenderObject(const RenderWindow* window, ShaderManager* shaderManager) 
		: window(window), shaderManager(shaderManager) {}

	virtual void init(const Camera* const camera) = 0;
	virtual void update(const Camera* const camera) = 0;
	virtual void handleInput(const InputManager& input) = 0;
	virtual void quit() = 0;
};


}


#endif