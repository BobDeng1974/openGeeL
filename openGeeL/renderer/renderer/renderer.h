#ifndef RENDERER_H
#define RENDERER_H

#include <mutex>
#include <queue>
#include <vector>
#include "utility/listener.h"
#include "threading.h"

namespace geeL {

	class IFrameBuffer;
	class DynamicBuffer;
	class RenderContext;
	class RenderWindow;
	class RenderScene;
	class SceneControlObject;
	class LightManager;
	class MeshDrawer;
	class MeshFactory;
	class Model;
	class GUIRenderer;


	//Basic interface for all rendering classes
	class Drawer {

	public:
		virtual void draw() = 0;

		void setParent(DynamicBuffer& buffer);
		const DynamicBuffer* const getParentBuffer() const;
		DynamicBuffer* const getParentBuffer();

	protected:
		DynamicBuffer* parentBuffer = nullptr;

	};


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

		GUIRenderer*  gui;
		RenderScene* scene;

		void updateGLStructures();

	private:
		std::queue<Model*> toAdd;
		std::queue<Model*> toRemove;

	};
}

#endif
