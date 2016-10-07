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

	void Renderer::initObjects() {
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->init();
		}
	}

	void Renderer::setLightManager(const LightManager& manager) {
		lightManager = &manager;
	}

	void Renderer::setMeshDrawer(const MeshDrawer& drawer) {
		meshDrawer = &drawer;
	}

	void Renderer::setShaderManager(const ShaderManager& manager) {
		shaderManager = &manager;
	}
}