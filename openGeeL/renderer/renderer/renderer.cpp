#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "cameras/camera.h"
#include "meshes/model.h"
#include "meshes/meshfactory.h"
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


	Renderer::Renderer(RenderWindow& window, RenderContext& context, MeshFactory& factory)
		: ThreadedObject(100L)
		, window(window)
		, context(context)
		, factory(factory)
		, gui(nullptr) {
	
		factory.addListener(*this);
	}


	void Renderer::addGUIRenderer(GUIRenderer* renderer) {
		gui = renderer;
	}

	void Renderer::setScene(RenderScene& scene) {
		this->scene = &scene;
	}


	void Renderer::onAdd(Model& model) {
		std::lock_guard<std::mutex> guard(glMutex);
		model.initGL();
	}

	void Renderer::onRemove(Model& model) {
		std::lock_guard<std::mutex> guard(glMutex);
		model.clearGL();
	}



}