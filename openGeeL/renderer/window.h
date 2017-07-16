#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include "utility\resolution.h"

struct GLFWwindow;

namespace geeL {

	class SceneCamera;
	class Debugger;


	enum class WindowMode {
		Windowed,
		ResizableWindow,
		BorderlessWindow,
		Fullscreen
	};


	class RenderWindow {

	public:
		GLFWwindow* glWindow;
		const char* name;
		const Resolution resolution;

		RenderWindow(const char* name, Resolution resolution, WindowMode mode = WindowMode::Windowed);
		~RenderWindow();

		void RenderWindow::swapBuffer() const;
		bool RenderWindow::shouldClose() const;
		void RenderWindow::close() const;

		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		Debugger* debugger;

		RenderWindow(const RenderWindow& other) = delete;
		RenderWindow& operator= (const RenderWindow& other) = delete;

	};


	inline unsigned int RenderWindow::getWidth() const {
		return resolution.getWidth();
	}

	inline unsigned int RenderWindow::getHeight() const {
		return resolution.getHeight();
	}


}

#endif
