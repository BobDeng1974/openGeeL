#ifndef RENDERTIME_H
#define RENDERTIME_H

#include <glfw3.h>


namespace geeL {

class Time {

public:
	static void update();

	static float deltaTime;

private:
	static float lastFrame;

};

}

#endif

