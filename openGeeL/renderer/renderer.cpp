#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "cameras/camera.h"
#include "renderer/rendercontext.h"
#include "framebuffer/framebuffer.h"

namespace geeL{

	void Drawer::setParent(DynamicBuffer& buffer) {
		parentBuffer = &buffer;
	}

	const DynamicBuffer* const Drawer::getParentBuffer() const {
		return parentBuffer;
	}

	DynamicBuffer* const Drawer::getParentBuffer() {
		return parentBuffer;
	}


	Renderer::Renderer(RenderWindow& window, RenderContext& context)
		: ThreadedObject(100L), window(&window), context(&context), gui(nullptr) {}


	void Renderer::addGUIRenderer(GUIRenderer* renderer) {
		gui = renderer;
	}

	void Renderer::setScene(RenderScene& scene) {
		this->scene = &scene;
	}

}