#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "utility/debug.h"
#include "window.h"


namespace geeL {

	RenderWindow::RenderWindow(const char* name, unsigned int width, unsigned int height, bool resizable) 
		: name(name), width(width), height(height), debugger(nullptr) {
	    
		glfwInit();
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	    glfwWindowHint(GLFW_RESIZABLE, resizable);

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		debugger = new Debugger();
		debugger->init();
#endif

	    glWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
	    if (glWindow == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	    }

	    glfwMakeContextCurrent(glWindow);

	    glewExperimental = GL_TRUE;
	    if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
	    }
	}

	RenderWindow::~RenderWindow() {
		if (debugger != nullptr)
			delete debugger;
	}


	void RenderWindow::swapBuffer() const {
	    glfwSwapBuffers(glWindow);
	}

	bool RenderWindow::shouldClose() const {
	    return glfwWindowShouldClose(glWindow) != 0;
	}

	void RenderWindow::close() const {
	    glfwTerminate();
	}
}
