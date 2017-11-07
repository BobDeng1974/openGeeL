#ifndef MASKING_H
#define MASKING_H

#include "shader/defshading.h"

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

		//Returns an equivalent render mask to given shading method 
		//(If it exists, otherwise default mask)
		static RenderMask getShadingMask(ShadingMethod method);

	};

}

#endif
