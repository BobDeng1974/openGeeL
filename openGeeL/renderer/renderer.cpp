#include "renderer.h"
#include "window.h"
#include "scripting\scenecontrolobject.h"
#include "inputmanager.h"
#include "cameras/camera.h"
#include "renderer\rendercontext.h"


namespace geeL{

	Renderer::Renderer(RenderWindow& window, InputManager& inputManager, RenderContext& context)
		: window(&window), inputManager(&inputManager), context(&context), gui(nullptr) {}


	void Renderer::addGUIRenderer(GUIRenderer* renderer) {
		gui = renderer;
	}

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

}