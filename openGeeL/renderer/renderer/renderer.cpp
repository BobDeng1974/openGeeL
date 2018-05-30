#include "renderer.h"
#include "scene.h"
#include "window.h"
#include "cameras/camera.h"
#include "materials/materialfactory.h"
#include "meshes/model.h"
#include "meshes/meshfactory.h"
#include "renderer/rendercontext.h"
#include "framebuffer/framebuffer.h"

namespace geeL {

	Renderer::Renderer(RenderWindow& window, 
		RenderContext& context, 
		MeshFactory& meshFactory, 
		MaterialFactory& materialFactory)
			: ThreadedObject(100L)
			, window(window)
			, context(context)
			, meshFactory(meshFactory)
			, materialFactory(materialFactory)
			, gui(nullptr) {
	
		meshFactory.addListener(*this);
		materialFactory.addListener(*this);
	}

	Renderer::~Renderer() {
		onRemove();
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

	void Renderer::addGUIRenderer(GUIRenderer& renderer) {
		gui = &renderer;
	}

	void Renderer::onAdd(GLStructure& structure) {
		std::lock_guard<std::mutex> guard(glMutex);
		toAdd.emplace(&structure);
	}

	void Renderer::onRemove(std::shared_ptr<GLStructure> structure) {
		std::lock_guard<std::mutex> guard(glMutex);
		toRemove.emplace(structure);
	}

	void Renderer::updateGLStructures() {
		onAdd();
		onRemove();
	}

	void Renderer::onAdd() {
		while (!toAdd.empty()) {
			GLStructure* structure = toAdd.front();
			structure->initGL();

			toAdd.pop();
		}
	}

	void Renderer::onRemove(){
		while (!toRemove.empty()) {
			std::shared_ptr<GLStructure> structure = toRemove.front();
			structure->clearGL();

			toRemove.pop();
		}
	}

}