#ifndef RENDERTIME_H
#define RENDERTIME_H

#include <glfw3.h>

namespace geeL {

	class Time {

	public:
		static float deltaTime;

		static void update();

	private:
		static float lastFrame;

	};
}

#endif

