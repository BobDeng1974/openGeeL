#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

namespace geeL {

	class FrameBuffer;
	class RenderContext;
	class RenderWindow;
	class RenderScene;
	class SceneControlObject;
	class InputManager;
	class LightManager;
	class MeshDrawer;
	class RenderPipeline;
	class GUIRenderer;


	//Basic interface for all rendering classes
	class Drawer {

	public:
		virtual void draw() = 0;

		void setParent(const FrameBuffer& buffer);
		const FrameBuffer* const getParentBuffer() const;
		unsigned int getParentFBO() const;

	protected:
		const FrameBuffer* parentBuffer = nullptr;

	};


	class Renderer : public Drawer {

	public:
		Renderer(RenderWindow& window, InputManager& inputManager, RenderContext& context);

		//Render scene until termination
		virtual void render() = 0;

		//Render single frame
		virtual void draw() = 0;

		virtual void handleInput() = 0;

		virtual void addGUIRenderer(GUIRenderer* renderer);
		virtual void addObject(SceneControlObject* obj);
		virtual void initSceneObjects();
	
		virtual void setScene(RenderScene& scene);


	protected:
		RenderContext* context;
		RenderWindow* window;
		InputManager* inputManager;
		GUIRenderer*  gui;

		RenderScene* scene;
		std::vector<SceneControlObject*> objects;

		void updateSceneControlObjects();

	};
}

#endif
