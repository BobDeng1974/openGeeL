#ifndef MASKING_H
#define MASKING_H

namespace geeL {

	enum class RenderMask {
		None = 0,
		Character = 1

	};

	class Masking {

	public:
		static void drawMask(RenderMask mask);
		static void readMask(RenderMask mask);

	};

}

#endif
