#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

#include "../renderer.h"

namespace geeL {

	class Camera;

	class SimpleRenderer : public Renderer {

	public:
		SimpleRenderer(RenderWindow* window, InputManager* inputManager);

		virtual void init();
		virtual void render();
		virtual void draw();
		virtual void handleInput();

		void setCamera(Camera* camera);

	private:
		Camera* currentCamera;

	};
}

#endif
