#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
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
	class RenderPipeline;
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


	class Renderer : public Drawer, public ThreadedObject {

	public:
		Renderer(RenderWindow& window, RenderContext& context);
		virtual ~Renderer() {}

		//Render single frame
		virtual void draw() = 0;

		virtual void addGUIRenderer(GUIRenderer* renderer);
		virtual void setScene(RenderScene& scene);
		virtual void linkInformation() const {}

	protected:
		RenderContext* context;
		RenderWindow* window;
		GUIRenderer*  gui;

		RenderScene* scene;

	};
}

#endif
