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


	class Renderer : public ThreadedObject, public DataEventListener<Model> {

	public:
		Renderer(RenderWindow& window, RenderContext& context, MeshFactory& factory);
		virtual ~Renderer() {}

		virtual void runStart();
		virtual void run();

		virtual void addGUIRenderer(GUIRenderer* renderer);
		virtual void setScene(RenderScene& scene);

		virtual void onAdd(Model& model);
		virtual void onRemove(Model& model);

	protected:
		mutable std::mutex glMutex;

		RenderContext& context;
		RenderWindow& window;
		MeshFactory& factory;

		GUIRenderer* gui;
		RenderScene* scene;

		void updateGLStructures();

	private:
		std::queue<Model*> toAdd;
		std::queue<Model*> toRemove;

	};
}

#endif
