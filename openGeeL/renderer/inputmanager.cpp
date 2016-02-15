#include "inputmanager.h"
#include "window.h"
#include <iostream>

namespace geeL {

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		manager->callKey(window, key, scancode, action, mode);
	}

	void scrollCallback(GLFWwindow* window, double x, double y) {
		InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		manager->callScroll(window, x, y);
	}


	//Forwards the input manager (as user pointer) and the callbacks to GLFW 
	void InputManager::init(const RenderWindow* renderWindow) {
		window = renderWindow;
		mouseX = window->width * 0.5f;
		mouseY = window->height * 0.5f;
		lastX = mouseX;
		lastY = mouseY;
		scroll = 0.0;

		glfwSetWindowUserPointer(window->glWindow, this);
		glfwSetKeyCallback(window->glWindow, keyboardCallback);
		glfwSetScrollCallback(window->glWindow , scrollCallback);
	}

	void InputManager::update() {
		//switch key buffers
		int* temp = currentKeys;
		currentKeys = previousKeys;

		for (size_t i = 0; i < maxKeys; i++)
			currentKeys[i] = temp[i];

		previousKeys = temp;

		//update mouse cursor information
		lastX = mouseX;
		lastY = mouseY;
		glfwGetCursorPos(window->glWindow, &mouseX, &mouseY);
	}
	

	//Add a callback to to the input manager that will be called during runtime
	void InputManager::addCallback(GLFWkeyfun callback) {
		callbacks.push_back(callback);
	}

	//Callback function that will call every registered callback and updates information about each key
	void InputManager::callKey(GLFWwindow* window, int key, int scancode, int action, int mode) {

		for (size_t i = 0; i < callbacks.size(); i++) {
			const GLFWkeyfun& callback = callbacks[i];
			callback(window, key, scancode, action, mode);
		}

		if (key > 0 && key < maxKeys) {
			currentKeys[key] = action;
		}
	}

	void InputManager::callScroll(GLFWwindow* window, double x, double y) {
		scroll -= y;
	}

	bool InputManager::getKey(int key) const {
		return currentKeys[key] == GLFW_PRESS;
	}

	bool InputManager::getKeyDown(int key) const {
		int previousAction = previousKeys[key];
		int currentAction = currentKeys[key];

		return previousAction == GLFW_RELEASE && currentAction == GLFW_PRESS;
	}

	bool InputManager::getKeyUp(int key) const {
		int previousAction = previousKeys[key];
		int currentAction = currentKeys[key];

		return previousAction == GLFW_PRESS && currentAction == GLFW_RELEASE;
	}

	bool InputManager::getKeyHold(int key) const {
		return currentKeys[key] == GLFW_REPEAT;
	}

	void InputManager::defineButton(string name, int key) {

		if (buttonMapping.count(name)) {
			vector<int> keyList = buttonMapping[name];
			keyList.push_back(key);
		}
		else {
			vector<int> keyList;
			keyList.push_back(key);

			buttonMapping[name] = keyList;
		}
	}

	bool InputManager::getButtonHelper(string button, KeyAction keyFunction) const {
		map<string, vector<int>>::const_iterator iterator = buttonMapping.find(button);

		if (iterator != buttonMapping.end()) {
			vector<int> list = iterator->second;

			for (size_t i = 0; i < list.size(); i++) {
				if ((this->*keyFunction)(list[i]))
					return true;
			}
		}

		return false;
	}

	bool InputManager::getButton(string button) const {
		return getButtonHelper(button, &InputManager::getKey);
	}

	bool InputManager::getButtonDown(string button) const {
		return getButtonHelper(button, &InputManager::getKeyDown);
	}

	bool InputManager::getButtonUp(string button) const {
		return getButtonHelper(button, &InputManager::getKeyUp);
	}

	bool InputManager::getButtonHold(string button) const {
		return getButtonHelper(button, &InputManager::getKeyHold);
	}

	double InputManager::getMouseX() const {
		return mouseX;
	}

	double InputManager::getMouseY() const {
		return mouseY;
	}

	double InputManager::getMouseXNorm() const {
		return mouseX / (double)window->width;
	}

	double InputManager::getMouseYNorm() const {
		return mouseY / (double)window->height;
	}

	double InputManager::getMouseXOffset() const {
		return (mouseX - lastX);
	}

	double InputManager::getMouseYOffset() const {
		return (lastY - mouseY);
	}

	double InputManager::getMouseScroll() const {
		return scroll;
	}

}