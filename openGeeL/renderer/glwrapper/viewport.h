#ifndef VIEWPORT_H
#define VIEWPORT_H

typedef unsigned int uint;

namespace geeL {

	class Viewport {

	public:
		static void set(uint x, uint y, uint width, uint height);
		static void setForced(uint x, uint y, uint width, uint height);

	private:
		static uint _x, _y, _width, _height;

	};

}

#endif