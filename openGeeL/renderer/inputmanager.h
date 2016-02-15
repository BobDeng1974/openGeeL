#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <glfw3.h>
#include <vector>
#include <map>
#include <iostream>

#define maxKeys 400

using namespace std;

namespace geeL {

class RenderWindow;
class RenderObject;

class InputManager {

public:

	typedef bool (InputManager::*KeyAction)(int) const;

	InputManager() {}

	void init(const RenderWindow* renderWindow);
	void update();

	void addCallback(GLFWkeyfun callback);

	void callKey(GLFWwindow* window, int key, int scancode, int action, int mode);
	void callScroll(GLFWwindow* window, double x, double y);

	bool getKey(int key) const;
	bool getKeyDown(int key) const;
	bool getKeyUp(int key) const;
	bool getKeyHold(int key) const;

	void defineButton(string name, int key);
	bool getButton(string button) const;
	bool getButtonDown(string button) const;
	bool getButtonUp(string button) const;
	bool getButtonHold(string button) const;

	double getMouseX() const;
	double getMouseY() const;
	double getMouseXNorm() const;
	double getMouseYNorm() const;
	double getMouseXOffset() const;
	double getMouseYOffset() const;
	double getMouseScroll() const;

private:
	const RenderWindow* window;
	vector<GLFWkeyfun> callbacks;
	map<string, vector<int>> buttonMapping;

	int keyboardBuffer1[maxKeys];
	int keyboardBuffer2[maxKeys];

	int* currentKeys = keyboardBuffer1;
	int* previousKeys = keyboardBuffer2;

	double mouseX;
	double mouseY;
	double lastX;
	double lastY;
	double scroll;

	bool getButtonHelper(string button, KeyAction keyFunction) const;
	
};



}

#endif
