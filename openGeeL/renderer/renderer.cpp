#include "renderer.h"
#include "window.h"
#include "renderobject.h"
#include "inputmanager.h"
#include "cameras/camera.h"

namespace geeL{

	Renderer::Renderer(RenderWindow* window, InputManager* inputManager)
		: window(window), inputManager(inputManager) {}


	void Renderer::addObject(RenderObject* obj) {
		objects.push_back(obj);
	}
}