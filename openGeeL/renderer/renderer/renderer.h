#ifndef RENDERER_H
#define RENDERER_H

#include <mutex>
#include <queue>
#include "utility/listener.h"
#include "threading.h"
#include "glstructures.h"

namespace geeL {

	class Camera;
	class GUIRenderer;
	class MaterialFactory;
	class MeshFactory;
	class Model;
	class RenderContext;
	class RenderScene;
	class RenderWindow;


	class Renderer : public ThreadedObject, public DataEventListener<GLStructure> {

	public:
		Renderer(RenderWindow& window, RenderContext& context, 
			MeshFactory& meshFactory, MaterialFactory& materialFactory);
		virtual ~Renderer();

		virtual void runStart();
		virtual void run();

		//Render complete scene of current frame with forward rendering
		virtual void drawForward(const Camera& camera, bool forceGamma = false) const = 0;

		virtual void addGUIRenderer(GUIRenderer& renderer);
		virtual void setScene(RenderScene& scene);

		virtual void onAdd(GLStructure& structure);
		virtual void onRemove(std::shared_ptr<GLStructure> structure);

	protected:
		mutable std::mutex glMutex;

		RenderContext& context;
		RenderWindow& window;
		MeshFactory& meshFactory;
		MaterialFactory& materialFactory;

		GUIRenderer* gui;
		RenderScene* scene;

	private:
		std::queue<GLStructure*> toAdd;
		std::queue<std::shared_ptr<GLStructure>> toRemove;

		void updateGLStructures();
		void onAdd();
		void onRemove();

	};

}

#endif
