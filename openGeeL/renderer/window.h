#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

struct GLFWwindow;

namespace geeL {

class Camera;

class RenderWindow {

public:
	GLFWwindow* glWindow;
	const char* name;
	unsigned int width, height;

	RenderWindow(const char* name, unsigned int width, unsigned int height, bool resizable = false);

	void RenderWindow::swapBuffer();
	bool RenderWindow::shouldClose();
	void RenderWindow::close();
	
};

}

#endif
