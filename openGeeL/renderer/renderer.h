#ifndef RENDERER_H
#define REBDERER_H

#include <vector>

namespace geeL {

class RenderWindow;
class RenderObject;
class InputManager;

class Renderer {

public:
	Renderer(RenderWindow* window, InputManager* inputManager);
	~Renderer();

	void render();
	void renderFrame();
	void handleInput();
	void addObject(RenderObject* obj);

private:
	RenderWindow* window;
	InputManager* inputManager;
	
	std::vector<RenderObject*> objects;

};

}


#endif
