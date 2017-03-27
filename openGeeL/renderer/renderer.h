#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

namespace geeL {

	class RenderContext;
	class RenderWindow;
	class RenderScene;
	class SceneControlObject;
	class InputManager;
	class LightManager;
	class MeshDrawer;
	class ShaderInformationLinker;
	class GUIRenderer;


	//Basic interface for rendering classes
	class Drawer {

	public:
		unsigned int parentFBO = 0;

		virtual void draw() = 0;

		//FBO this object should be drawn to.
		//If the object used other fbo's itself it should activate
		//this one afterwards
		void setParentFBO(unsigned int fbo) {
			this->parentFBO = fbo;
		}
	};


	class Renderer : public Drawer {

	public:
		Renderer(RenderWindow& window, InputManager& inputManager, RenderContext& context);

		virtual void init() = 0;

		//Render scene until termination
		virtual void render() = 0;

		//Render single frame
		virtual void draw() = 0;

		virtual void handleInput() = 0;

		virtual void addGUIRenderer(GUIRenderer* renderer);
		virtual void addObject(SceneControlObject* obj);
		virtual void initObjects();
	
		virtual void setScene(RenderScene& scene);


	protected:
		RenderContext* context;
		RenderWindow* window;
		InputManager* inputManager;
		GUIRenderer*  gui;

		RenderScene* scene;
	
		std::vector<SceneControlObject*> objects;

	};
}

#endif
