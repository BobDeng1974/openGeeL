#include <glfw3.h>
#include "inputmanager.h"
#include "window.h"
#include <iostream>

using namespace std;

namespace geeL {

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		manager->callKey(window, key, scancode, action, mode);
	}

	void scrollCallback(GLFWwindow* window, double x, double y) {
		InputManager* manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
		manager->callScroll(window, x, y);
	}

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

		//Switch key buffers
		int* temp = currentKeys;
		for (size_t i = 0; i < maxKeys; i++)
			currentKeys[i] = temp[i];

		previousKeys = temp;

		//Update mouse cursor information
		lastX = mouseX;
		lastY = mouseY;
		glfwGetCursorPos(window->glWindow, &mouseX, &mouseY);
	}
	

	void InputManager::addCallback(std::function<void(GLFWwindow*, int, int, int, int)> function) {
		callbacks.push_back(function);
	}

	
	void InputManager::callKey(GLFWwindow* window, int key, int scancode, int action, int mode) {

		for (size_t i = 0; i < callbacks.size(); i++) {
			auto func = callbacks[i];
			func(window, key, scancode, action, mode);
		}

		if (key > 0 && key < maxKeys) {
			currentKeys[key] = action;
		}
	}

	void InputManager::callScroll(GLFWwindow* window, double x, double y) {
		lastScroll = scroll;
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

	bool InputManager::getMouseKey(int key) const {
		int state = glfwGetMouseButton(window->glWindow, GLFW_MOUSE_BUTTON_LEFT);
		return state == GLFW_PRESS;
	}

	float InputManager::getMouseX() const {
		return mouseX;
	}

	float InputManager::getMouseY() const {
		return mouseY;
	}

	float InputManager::getMouseXNorm() const {
		return mouseX / (double)window->width;
	}

	float InputManager::getMouseYNorm() const {
		return mouseY / (double)window->height;
	}

	float InputManager::getMouseXOffset() const {
		return (mouseX - lastX);
	}

	float InputManager::getMouseYOffset() const {
		return (lastY - mouseY);
	}

	float InputManager::getMouseScroll() const {
		return scroll;
	}

	float InputManager::getMouseScrollOffset() const {
		return scroll - lastScroll;
	}

}