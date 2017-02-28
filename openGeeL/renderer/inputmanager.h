#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

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

	class InputManager {

	public:

		typedef bool (InputManager::*KeyAction)(int) const;

		InputManager() {}

		//Forwards the input manager (as user pointer) and the callbacks to GLFW 
		void init(const RenderWindow* renderWindow);
		void update();

		//Add a callback to to the input manager that will be called during runtime
		void addCallback(std::function<void(GLFWwindow*, int, int, int, int)> function);

		//Callback function that will call every registered callback and updates information about each key
		void callKey(GLFWwindow* window, int key, int scancode, int action, int mode);
		void callScroll(GLFWwindow* window, double x, double y);

		bool getKey(int key) const;
		bool getKeyDown(int key) const;
		bool getKeyUp(int key) const;
		bool getKeyHold(int key) const;

		void defineButton(std::string name, int key);
		bool getButton(std::string button) const;
		bool getButtonDown(std::string button) const;
		bool getButtonUp(std::string button) const;
		bool getButtonHold(std::string button) const;

		bool getMouseKey(int key) const;
		float getMouseX() const;
		float getMouseY() const;
		float getMouseXNorm() const;
		float getMouseYNorm() const;
		float getMouseXOffset() const;
		float getMouseYOffset() const;
		float getMouseScroll() const;
		float getMouseScrollOffset() const;

	private:
		const RenderWindow* window;
		std::vector<std::function<void(GLFWwindow*, int, int, int, int)>> callbacks;
		std::map<std::string, std::vector<int>> buttonMapping;

		int keyboardBuffer1[maxKeys];
		int keyboardBuffer2[maxKeys];

		int* currentKeys = keyboardBuffer1;
		int* previousKeys = keyboardBuffer2;

		double mouseX;
		double mouseY;
		double lastX;
		double lastY;
		double scroll;
		double lastScroll;

		bool getButtonHelper(std::string button, KeyAction keyFunction) const;
	};
}

#endif
