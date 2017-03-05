#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

struct GLFWwindow;

namespace geeL {

	class Camera;
	class Debugger;

	class RenderWindow {

	public:
		GLFWwindow* glWindow;
		const char* name;
		unsigned int width, height;

		RenderWindow(const char* name, unsigned int width, unsigned int height, bool resizable = false);
		~RenderWindow();

		void RenderWindow::swapBuffer() const;
		bool RenderWindow::shouldClose() const;
		void RenderWindow::close() const;

	private:
		Debugger* debugger;
	};
}

#endif
