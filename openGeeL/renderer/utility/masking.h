#ifndef MASKING_H
#define MASKING_H

namespace geeL {

	enum class RenderMask {
		None = 0,
		Generic = 1,
		Forward = 2,
		Transparent = 3,
		Skin = 4

	};

	class Masking {

	public:
		static void drawMask(RenderMask mask);
		static void readMask(RenderMask mask);
		static void passthrough();

	};

}

#endif
