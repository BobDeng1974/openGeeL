#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <vector>
#include <map>

#define maxKeys 400
#define mouseKeys 7

using namespace std;

struct GLFWwindow;

namespace geeL {

typedef void(*InputCallback)(GLFWwindow*, int, int, int, int);

class RenderWindow;

class InputManager {

public:

	typedef bool (InputManager::*KeyAction)(int) const;

	InputManager() {}

	void init(const RenderWindow* renderWindow);
	void update();

	void addCallback(InputCallback callback);

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

	bool getMouseKey(int key) const;
	double getMouseX() const;
	double getMouseY() const;
	double getMouseXNorm() const;
	double getMouseYNorm() const;
	double getMouseXOffset() const;
	double getMouseYOffset() const;
	double getMouseScroll() const;
	double getMouseScrollOffset() const;

private:
	const RenderWindow* window;
	vector<InputCallback> callbacks;
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
	double lastScroll;

	bool getButtonHelper(string button, KeyAction keyFunction) const;
	
};



}

#endif
