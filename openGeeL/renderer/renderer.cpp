#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "scripting\scenecontrolobject.h"
#include "inputmanager.h"
#include "cameras/camera.h"
#include "renderer\rendercontext.h"
#include "framebuffer\framebuffer.h"

namespace geeL{

	void Drawer::setParent(IFrameBuffer& buffer) {
		parentBuffer = &buffer;
	}

	const IFrameBuffer* const Drawer::getParentBuffer() const {
		return parentBuffer;
	}

	IFrameBuffer * const Drawer::getParentBuffer() {
		return parentBuffer;
	}


	Renderer::Renderer(RenderWindow& window, InputManager& inputManager, RenderContext& context)
		: window(&window), inputManager(&inputManager), context(&context), gui(nullptr) {}


	void Renderer::addGUIRenderer(GUIRenderer* renderer) {
		gui = renderer;
	}

	void Renderer::addObject(SceneControlObject* obj) {
		objects.push_back(obj);
	}

	void Renderer::initSceneObjects() {
		for (size_t i = 0; i < objects.size(); i++) {
			objects[i]->init();
		}
	}

	void Renderer::setScene(RenderScene& scene) {
		this->scene = &scene;
	}

	void Renderer::updateSceneControlObjects() {
		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->draw(scene->getCamera());
	}

}