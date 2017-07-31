#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

namespace geeL {

	class ColorBuffer;
	class FrameBuffer;
	class IFrameBuffer;
	class RenderContext;
	class RenderWindow;
	class RenderScene;
	class SceneControlObject;
	class Input;
	class LightManager;
	class MeshDrawer;
	class RenderPipeline;
	class GUIRenderer;


	//Basic interface for all rendering classes
	class Drawer {

	public:
		virtual void draw() = 0;

		void setParent(IFrameBuffer& buffer);
		const IFrameBuffer* const getParentBuffer() const;
		IFrameBuffer* const getParentBuffer();

	protected:
		IFrameBuffer* parentBuffer = nullptr;

	};


	class Renderer : public Drawer {

	public:
		Renderer(RenderWindow& window, Input& inputManager, RenderContext& context);

		//Render scene until termination
		virtual void render() = 0;

		//Render single frame
		virtual void draw() = 0;

		virtual void handleInput() {};

		virtual void addGUIRenderer(GUIRenderer* renderer);
		virtual void setScene(RenderScene& scene);
		virtual void linkInformation() const {}

	protected:
		RenderContext* context;
		RenderWindow* window;
		Input& input;
		GUIRenderer*  gui;

		RenderScene* scene;

	};
}

#endif
