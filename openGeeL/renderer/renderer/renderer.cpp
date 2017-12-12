#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "cameras/camera.h"
#include "meshes/model.h"
#include "meshes/meshfactory.h"
#include "renderer/rendercontext.h"
#include "framebuffer/framebuffer.h"

namespace geeL{

	Renderer::Renderer(RenderWindow& window, RenderContext& context, MeshFactory& factory)
		: ThreadedObject(100L)
		, window(window)
		, context(context)
		, factory(factory)
		, gui(nullptr) {
	
		factory.addListener(*this);
	}


	void Renderer::runStart() {
		window.makeCurrent();

		std::lock_guard<std::mutex> glGuard(glMutex);
		updateGLStructures();
	}

	void Renderer::run() {
		std::lock_guard<std::mutex> glGuard(glMutex);
		updateGLStructures();
	}

	void Renderer::addGUIRenderer(GUIRenderer* renderer) {
		gui = renderer;
	}

	void Renderer::setScene(RenderScene& scene) {
		this->scene = &scene;
	}


	void Renderer::onAdd(GLStructure& structure) {
		std::lock_guard<std::mutex> guard(glMutex);
		toAdd.emplace(&structure);
	}

	void Renderer::onRemove(GLStructure& structure) {
		std::lock_guard<std::mutex> guard(glMutex);
		toRemove.emplace(&structure);
	}

	void Renderer::updateGLStructures() {
		while (!toAdd.empty()) {
			GLStructure* structure = toAdd.front();
			structure->initGL();

			toAdd.pop();
		}
		
		while (!toRemove.empty()) {
			GLStructure* structure = toRemove.front();
			structure->clearGL();

			toRemove.pop();
		}
	}
}