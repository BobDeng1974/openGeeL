#include <glfw3.h>
#include "inputmanager.h"
#include "window.h"
#include <iostream>

using namespace std;

namespace geeL {


	std::mutex inputMutex;


	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
		InputReader* manager = static_cast<InputReader*>(glfwGetWindowUserPointer(window));
		manager->callKey(window, key, scancode, action, mode);
	}

	void scrollCallback(GLFWwindow* window, double x, double y) {
		InputReader* manager = static_cast<InputReader*>(glfwGetWindowUserPointer(window));
		manager->callScroll(window, x, y);
	}


	void InputReader::init(const RenderWindow* renderWindow) {
		window = renderWindow;
		mouseX = window->getWidth() * 0.5f;
		mouseY = window->getHeight() * 0.5f;
		scroll = 0.0;

		glfwSetWindowUserPointer(window->glWindow, this);
		glfwSetKeyCallback(window->glWindow, keyboardCallback);
		glfwSetScrollCallback(window->glWindow, scrollCallback);
	}

	void InputReader::update() {
		inputMutex.lock();

		double tempX = mouseX;
		double tempY = mouseY;
		glfwGetCursorPos(window->glWindow, &tempX, &tempY);

		mouseX = tempX;
		mouseY = tempY;

		inputMutex.unlock();
	}

	void InputReader::callKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key > 0 && key < maxKeys) {
			keys[key] = action;
		}
	}

	void InputReader::callScroll(GLFWwindow* window, double x, double y) {
		scroll = scroll() - y;
	}

	bool InputReader::getKey(int key) const {
		return keys[key] == GLFW_PRESS;
	}


	bool InputReader::getKeyHold(int key) const {
		return keys[key] == GLFW_REPEAT;
	}

	bool InputReader::getMouseKey(int key) const {
		int state = glfwGetMouseButton(window->glWindow, GLFW_MOUSE_BUTTON_LEFT + key);
		return state == GLFW_PRESS;
	}

	double InputReader::getMouseX() const {
		return mouseX;
	}

	double InputReader::getMouseY() const {
		return mouseY;
	}

	double InputReader::getMouseXNorm() const {
		return mouseX / (double)window->getWidth();
	}

	double InputReader::getMouseYNorm() const {
		return mouseY / (double)window->getHeight();
	}

	double InputReader::getMouseScroll() const {
		return scroll;
	}


	

	InputManager::InputManager(InputReader& reader) 
		: inputReader(reader) {}


	void InputManager::update() {
		inputMutex.lock();

		for (size_t i = 0; i < maxKeys; i++)
			currentKeys[i] = inputReader.keys[i];

		lastX = mouseX;
		lastY = mouseY;
		mouseX = inputReader.getMouseX();
		mouseY = inputReader.getMouseY();

		lastScroll = scroll;
		scroll = inputReader.getMouseScroll();

		inputMutex.unlock();
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
		return inputReader.getMouseKey(key);
	}

	double InputManager::getMouseX() const {
		return mouseX;
	}

	double InputManager::getMouseY() const {
		return mouseY;
	}

	double InputManager::getMouseXNorm() const {
		return  inputReader.getMouseXNorm();
	}

	double InputManager::getMouseYNorm() const {
		return  inputReader.getMouseYNorm();
	}

	double InputManager::getMouseXOffset() const {
		return (mouseX - lastX);
	}

	double InputManager::getMouseYOffset() const {
		return (lastY - mouseY);
	}

	double InputManager::getMouseScroll() const {
		return inputReader.getMouseScroll();
	}

	double InputManager::getMouseScrollOffset() const {
		return scroll - lastScroll;
	}

}