#include "renderer.h"
#include "window.h"
#include "scripting\scenecontrolobject.h"
#include "inputmanager.h"
#include "cameras/camera.h"


namespace geeL{

	Renderer::Renderer(RenderWindow* window, InputManager* inputManager)
		: window(window), inputManager(inputManager) {}


	void Renderer::addObject(SceneControlObject* obj) {
		objects.push_back(obj);
	}

	void Renderer::initObjects() {
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->init();
		}
	}

	void Renderer::setScene(RenderScene& scene) {
		this->scene = &scene;
	}

	void Renderer::setShaderManager(const ShaderManager& manager) {
		shaderManager = &manager;
	}
}