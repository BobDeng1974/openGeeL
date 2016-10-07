#include "renderer.h"
#include "window.h"
#include "renderobject.h"
#include "inputmanager.h"
#include "cameras/camera.h"


namespace geeL{

	Renderer::Renderer(RenderWindow* window, InputManager* inputManager)
		: window(window), inputManager(inputManager) {}


	void Renderer::addObject(SceneObject* obj) {
		objects.push_back(obj);
	}

	void Renderer::initObjects() {
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->init();
		}
	}

	void Renderer::setScene(const RenderScene& scene) {
		this->scene = &scene;
	}

	void Renderer::setShaderManager(const ShaderManager& manager) {
		shaderManager = &manager;
	}
}