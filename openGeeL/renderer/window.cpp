#include <iostream>
#include <glfw3.h>
#include "window.h"


namespace geeL {

	RenderWindow::RenderWindow(const char* name, unsigned int width, unsigned int height, bool resizable) : name(name), width(width), height(height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, resizable);

		glWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
		if (glWindow == nullptr) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}

		glfwMakeContextCurrent(glWindow);
	}
}