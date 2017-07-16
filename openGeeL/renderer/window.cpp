#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "utility/debug.h"
#include "window.h"


namespace geeL {

	RenderWindow::RenderWindow(const char* name, Resolution resolution, WindowMode mode)
		: name(name), debugger(nullptr), resolution(resolution) {
	    
		glfwInit();
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	    

#ifdef _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		debugger = new Debugger();
		debugger->init();
#endif

		switch (mode) {
			case WindowMode::Windowed: 
				glfwWindowHint(GLFW_RESIZABLE, false);
				glWindow = glfwCreateWindow(resolution.getWidth(), resolution.getHeight(), name, nullptr, nullptr);
				break;
			case WindowMode::ResizableWindow:
				glfwWindowHint(GLFW_RESIZABLE, true);
				glWindow = glfwCreateWindow(resolution.getWidth(), resolution.getHeight(), name, nullptr, nullptr);
				break;
			case WindowMode::BorderlessWindow: {
				auto monitor = glfwGetPrimaryMonitor();

				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwWindowHint(GLFW_RED_BITS, mode->redBits);
				glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
				glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

				glWindow = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
			}
				break;
			case WindowMode::Fullscreen:
				glWindow = glfwCreateWindow(resolution.getWidth(), resolution.getHeight(), name, glfwGetPrimaryMonitor(), nullptr);
				break;
		}

	    if (glWindow == nullptr) {
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
	    }

	    glfwMakeContextCurrent(glWindow);

	    glewExperimental = GL_TRUE;
	    if (glewInit() != GLEW_OK) {
			std::cout << "Failed to initialize GLEW" << std::endl;
	    }

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
