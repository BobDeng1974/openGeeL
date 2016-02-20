#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

namespace geeL {

class RenderWindow;
class RenderObject;
class InputManager;
class ShaderManager;

class Renderer {

public:
	Renderer(RenderWindow* window, InputManager* inputManager, ShaderManager* shaderManger);

	virtual void init() = 0;
	virtual void render() = 0;
	virtual void renderFrame() = 0;
	virtual void handleInput() = 0;
	virtual void addObject(RenderObject* obj);


protected:
	RenderWindow* window;
	InputManager* inputManager;
	ShaderManager* shaderManger;
	
	std::vector<RenderObject*> objects;

};

}


#endif
