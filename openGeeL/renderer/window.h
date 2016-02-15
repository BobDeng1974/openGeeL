#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

struct GLFWwindow;

namespace geeL {

class RenderWindow {

public:
	GLFWwindow* glWindow;
	const char* name;
	unsigned int width, height;

	RenderWindow(const char* name, unsigned int width, unsigned int height, bool resizable = false);

	inline void RenderWindow::swapBuffer() {
		glfwSwapBuffers(glWindow);
	}

	inline bool RenderWindow::shouldClose() {
		return glfwWindowShouldClose(glWindow);
	}

	inline void RenderWindow::close() {
		glfwTerminate();
	}
	
};

}

#endif
