#include "renderer.h"
#include "window.h"
#include "renderobject.h"
#include "inputmanager.h"
#include "shader/shadermanager.h"
#include "cameras/camera.h"

namespace geeL{

	Renderer::Renderer(RenderWindow* window, InputManager* inputManager, ShaderManager* shaderManger)
		: window(window), inputManager(inputManager), shaderManger(shaderManger) {}


	void Renderer::addObject(RenderObject* obj) {
		objects.push_back(obj);
	}
}