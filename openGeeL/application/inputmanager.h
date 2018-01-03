#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <vector>
#include <map>
#include <functional>
#include <mutex>
#include "objectwrapper.h"

#define maxKeys 400
#define mouseKeys 7

struct GLFWwindow;

namespace geeL {

	class RenderWindow;


	//Interface for all input managers
	class Input {

	public:
		virtual bool getKey(int key) const = 0;
		virtual bool getKeyDown(int key) const = 0;
		virtual bool getKeyUp(int key) const = 0;
		virtual bool getKeyHold(int key) const = 0;

		virtual bool getButton(const std::string& button) const = 0;
		virtual bool getButtonDown(const std::string& button) const = 0;
		virtual bool getButtonUp(const std::string& button) const = 0;
		virtual bool getButtonHold(const std::string& button) const = 0;

		virtual bool getMouseKey(int key) const = 0;
		virtual double getMouseX() const = 0;
		virtual double getMouseY() const = 0;
		virtual double getMouseXNorm() const = 0;
		virtual double getMouseYNorm() const = 0;
		virtual double getMouseXOffset() const = 0;
		virtual double getMouseYOffset() const = 0;
		virtual double getMouseScroll() const = 0;
		virtual double getMouseScrollOffset() const = 0;

	};


	class InputManager : public Input {

	public:
		typedef bool (InputManager::*KeyAction)(int) const;

		//Forwards the input manager (as user pointer) and the callbacks to GLFW 
		void init(const RenderWindow* renderWindow);
		void update();

		//Callback function that will call every registered callback and updates information about each key
		void callKey(GLFWwindow* window, int key, int scancode, int action, int mode);
		void callScroll(GLFWwindow* window, double x, double y);

		virtual bool getKey(int key) const;
		virtual bool getKeyDown(int key) const;
		virtual bool getKeyUp(int key) const;
		virtual bool getKeyHold(int key) const;

		void defineButton(const std::string& name, int key);

		virtual bool getButton(const std::string& button) const;
		virtual bool getButtonDown(const std::string& button) const;
		virtual bool getButtonUp(const std::string& button) const;
		virtual bool getButtonHold(const std::string& button) const;

		virtual bool getMouseKey(int key) const;
		virtual double getMouseX() const;
		virtual double getMouseY() const;
		virtual double getMouseXNorm() const;
		virtual double getMouseYNorm() const;
		virtual double getMouseXOffset() const;
		virtual double getMouseYOffset() const;
		virtual double getMouseScroll() const;
		virtual double getMouseScrollOffset() const;

	private:
		const RenderWindow* window;

		std::map<std::string, std::vector<int>> buttonMapping;

		AtomicWrapper<int> keyboardBuffer1[maxKeys];
		AtomicWrapper<int> keyboardBuffer2[maxKeys];

		AtomicWrapper<int>* currentKeys = keyboardBuffer1;
		AtomicWrapper<int>* previousKeys = keyboardBuffer2;

		AtomicWrapper<double> mouseX;
		AtomicWrapper<double> mouseY;
		AtomicWrapper<double> lastX;
		AtomicWrapper<double> lastY;
		AtomicWrapper<double> scroll;
		AtomicWrapper<double> lastScroll;

		bool getButtonHelper(const std::string& button, KeyAction keyFunction) const;

	};
}

#endif
