#ifndef GEEL_APPLICATION_H
#define GEEL_APPLICATION_H

namespace geeL {

	class Renderer;
	class RenderWindow;
	class InputManager;


	class Application {

	public:
		Application(RenderWindow& window, InputManager& inputManager, Renderer& renderer);

		void run();

		static bool closing();

	private:
		RenderWindow& window;
		InputManager& inputManager;
		Renderer& renderer;


	};

}

#endif
