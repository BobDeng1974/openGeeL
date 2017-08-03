#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "inputmanager.h"
#include "cameras/camera.h"
#include "renderer/rendercontext.h"
#include "framebuffer/framebuffer.h"

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


	Renderer::Renderer(RenderWindow& window, Input& inputManager, RenderContext& context)
		: ThreadedObject(100L), window(&window), input(inputManager), context(&context), gui(nullptr) {}


	void Renderer::addGUIRenderer(GUIRenderer* renderer) {
		gui = renderer;
	}

	void Renderer::setScene(RenderScene& scene) {
		this->scene = &scene;
	}

}