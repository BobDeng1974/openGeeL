#define GLEW_STATIC
#include <glew.h>
#include "viewport.h"

namespace geeL {

	uint Viewport::_x = 0;
	uint Viewport::_y = 0;
	uint Viewport::_width = 0;
	uint Viewport::_height = 0;

	void Viewport::set(uint x, uint y, uint width, uint height) {
		bool changed = false;

		if (x != Viewport::_x) {
			Viewport::_x = x;
			changed = true;
		}

		if (y != Viewport::_y) {
			Viewport::_y = y;
			changed = true;
		}

		if (width != Viewport::_width) {
			Viewport::_width = width;
			changed = true;
		}

		if (height != Viewport::_height) {
			Viewport::_height = height;
			changed = true;
		}

		if(changed)
			glViewport(x, y, width, height);

	}

	void Viewport::setForced(uint x, uint y, uint width, uint height) {
		Viewport::_x = x;
		Viewport::_y = y;
		Viewport::_width = width;
		Viewport::_height = height;

		glViewport(x, y, width, height);
	}

}