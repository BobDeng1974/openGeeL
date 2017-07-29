#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "../application/objectwrapper.h"
#include <vector>
#include <map>
#include <functional>

#define maxKeys 400
#define mouseKeys 7

struct GLFWwindow;

namespace geeL {

	class RenderWindow;

	/*
	template<class T>
	class CallbackDelegate {

	public:
		typedef void(T::*Callback)(GLFWwindow*, int, int, int, int);

		CallbackDelegate(T& obj, Callback callback) 
			: obj(obj), callback(callback) {}


		void Run(GLFWwindow* window, int key, int scancode, int action, int mode) {
			obj.*callback(window, key, scancode, action, mode);
		}

	private:
		CallbackDelegate();

		T& obj;
		Callback callback;
	};
	*/

	//Interface for all input managers
	class Input {

	public:
		//Add a callback to to the input manager that will be called during runtime
		virtual void addCallback(std::function<void(GLFWwindow*, int, int, int, int)> function) = 0;

		virtual bool getKey(int key) const = 0;
		virtual bool getKeyDown(int key) const = 0;
		virtual bool getKeyUp(int key) const = 0;
		virtual bool getKeyHold(int key) const = 0;

		virtual bool getButton(std::string button) const = 0;
		virtual bool getButtonDown(std::string button) const = 0;
		virtual bool getButtonUp(std::string button) const = 0;
		virtual bool getButtonHold(std::string button) const = 0;

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

		InputManager() {}

		//Forwards the input manager (as user pointer) and the callbacks to GLFW 
		void init(const RenderWindow* renderWindow);
		void update();

		//Add a callback to to the input manager that will be called during runtime
		virtual void addCallback(std::function<void(GLFWwindow*, int, int, int, int)> function);

		//Callback function that will call every registered callback and updates information about each key
		void callKey(GLFWwindow* window, int key, int scancode, int action, int mode);
		void callScroll(GLFWwindow* window, double x, double y);

		virtual bool getKey(int key) const;
		virtual bool getKeyDown(int key) const;
		virtual bool getKeyUp(int key) const;
		virtual bool getKeyHold(int key) const;

		void defineButton(std::string name, int key);

		virtual bool getButton(std::string button) const;
		virtual bool getButtonDown(std::string button) const;
		virtual bool getButtonUp(std::string button) const;
		virtual bool getButtonHold(std::string button) const;

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
		std::vector<std::function<void(GLFWwindow*, int, int, int, int)>> callbacks;
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

		bool getButtonHelper(std::string button, KeyAction keyFunction) const;
	};
}

#endif
