#ifndef RENDERER_H
#define RENDERER_H

#include <mutex>
#include <queue>
#include "utility/listener.h"
#include "threading.h"
#include "glstructures.h"

namespace geeL {

	class GUIRenderer;
	class MeshFactory;
	class Model;
	class RenderContext;
	class RenderScene;
	class RenderWindow;


	class Renderer : public ThreadedObject, public DataEventListener<GLStructure> {

	public:
		Renderer(RenderWindow& window, RenderContext& context, MeshFactory& factory);
		virtual ~Renderer() {}

		virtual void runStart();
		virtual void run();

		virtual void addGUIRenderer(GUIRenderer* renderer);
		virtual void setScene(RenderScene& scene);

		virtual void onAdd(GLStructure& structure);
		virtual void onRemove(GLStructure& structure);

	protected:
		mutable std::mutex glMutex;

		RenderContext& context;
		RenderWindow& window;
		MeshFactory& factory;

		GUIRenderer* gui;
		RenderScene* scene;

	private:
		std::queue<GLStructure*> toAdd;
		std::queue<GLStructure*> toRemove;

		void updateGLStructures();

	};

}

#endif
