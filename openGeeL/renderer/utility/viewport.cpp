#define GLEW_STATIC
#include <glew.h>
#include "viewport.h"

namespace geeL {

	static uint _x = 0;
	static uint _y = 0;
	static uint _width = 0;
	static uint _height = 0;

	void Viewport::set(uint x, uint y, uint width, uint height) {
		bool changed = false;

		if (x != _x) {
			_x = x;
			changed = true;
		}

		if (y != _y) {
			_y = y;
			changed = true;
		}

		if (width != _width) {
			_width = width;
			changed = true;
		}

		if (height != _height) {
			_height = height;
			changed = true;
		}

		if(changed)
			glViewport(x, y, width, height);

	}

	void Viewport::setForced(uint x, uint y, uint width, uint height) {
		_x = x; 
		_y = y; 
		_width = width; 
		_height = height;

		glViewport(x, y, width, height);
	}

}