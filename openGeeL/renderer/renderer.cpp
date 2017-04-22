#include "renderer.h"
#include "window.h"
#include "scripting\scenecontrolobject.h"
#include "inputmanager.h"
#include "cameras/camera.h"
#include "renderer\rendercontext.h"
#include "framebuffer\framebuffer.h"

namespace geeL{

	void Drawer::setParent(const FrameBuffer& buffer) {
		parentBuffer = &buffer;
	}

	const FrameBuffer* const Drawer::getParentBuffer() const {
		return parentBuffer;
	}

	unsigned int Drawer::getParentFBO() const {
		if (parentBuffer != nullptr)
			return parentBuffer->getFBO();

		return 0;
	}


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