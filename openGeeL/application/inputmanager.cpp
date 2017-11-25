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
		mouseX = window->getWidth() * 0.5f;
		mouseY = window->getHeight() * 0.5f;
		lastX = mouseX;
		lastY = mouseY;
		scroll = 0.0;

		glfwSetWindowUserPointer(window->glWindow, this);
		glfwSetKeyCallback(window->glWindow, keyboardCallback);
		glfwSetScrollCallback(window->glWindow , scrollCallback);
	}

	void InputManager::update() {

		//Switch key buffers
		AtomicWrapper<int>* temp = currentKeys;
		for (size_t i = 0; i < maxKeys; i++)
			currentKeys[i] = temp[i];

		previousKeys = temp;

		//Update mouse cursor information
		lastX = mouseX;
		lastY = mouseY;

		double tempX = mouseX;
		double tempY = mouseY;
		glfwGetCursorPos(window->glWindow, &tempX, &tempY);

		mouseX = tempX;
		mouseY = tempY;
	}
	

	void InputManager::addCallback(std::function<void(int, int, int, int)> function) {
		callbackMutex.lock();
		callbacks.push_back(function);
		callbackMutex.unlock();
	}

	
	void InputManager::callKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
		callbackMutex.lock();
		for (size_t i = 0; i < callbacks.size(); i++) {
			auto func = callbacks[i];
			func(key, scancode, action, mode);
		}
		callbackMutex.unlock();

		if (key > 0 && key < maxKeys) {
			currentKeys[key] = action;
		}
	}

	void InputManager::callScroll(GLFWwindow* window, double x, double y) {
		lastScroll = scroll;
		scroll = scroll() - y;
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

	void InputManager::defineButton(const std::string& name, int key) {

		if (buttonMapping.count(name)) {
			vector<int>& keyList = buttonMapping[name];
			keyList.push_back(key);
		}
		else {
			vector<int> keyList;
			keyList.push_back(key);

			buttonMapping[name] = keyList;
		}
	}

	bool InputManager::getButtonHelper(const std::string& button, KeyAction keyFunction) const {
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

	bool InputManager::getButton(const std::string& button) const {
		return getButtonHelper(button, &InputManager::getKey);
	}

	bool InputManager::getButtonDown(const std::string& button) const {
		return getButtonHelper(button, &InputManager::getKeyDown);
	}

	bool InputManager::getButtonUp(const std::string& button) const {
		return getButtonHelper(button, &InputManager::getKeyUp);
	}

	bool InputManager::getButtonHold(const std::string& button) const {
		return getButtonHelper(button, &InputManager::getKeyHold);
	}

	bool InputManager::getMouseKey(int key) const {
		int state = glfwGetMouseButton(window->glWindow, GLFW_MOUSE_BUTTON_LEFT + key);
		return state == GLFW_PRESS;
	}

	double InputManager::getMouseX() const {
		return mouseX;
	}

	double InputManager::getMouseY() const {
		return mouseY;
	}

	double InputManager::getMouseXNorm() const {
		return mouseX / (double)window->getWidth();
	}

	double InputManager::getMouseYNorm() const {
		return mouseY / (double)window->getHeight();
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

	double InputManager::getMouseScrollOffset() const {
		return scroll - lastScroll;
	}

}