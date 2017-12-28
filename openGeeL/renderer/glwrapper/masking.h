#ifndef MASKING_H
#define MASKING_H

#include <string>
#include "shader/defshading.h"

namespace geeL {

	enum class RenderMask {
		None = 0,
		Empty = 1,
		Forward = 2,
		Hybrid = 4,
		Transparent = 8,
		Skin = 16,

	};

	class Masking {

	public:
		static void drawMask(RenderMask mask);
		static void readMask(RenderMask mask);

		static void passthrough();

		//Returns an equivalent render mask to given shading method 
		//(If it exists, otherwise default mask)
		static RenderMask getShadingMask(ShadingMethod method);
		static RenderMask getShadingMask(const std::string& name);

	};



	inline RenderMask operator|(RenderMask a, RenderMask b) {
		return static_cast<RenderMask>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline RenderMask operator&(RenderMask a, RenderMask b) {
		return static_cast<RenderMask>(static_cast<int>(a) & static_cast<int>(b));
	}


}

#endif
