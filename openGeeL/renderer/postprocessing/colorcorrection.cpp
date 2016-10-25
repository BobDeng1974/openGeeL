#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "colorcorrection.h"

namespace geeL {

	ColorCorrection::ColorCorrection(float red, float green, float blue, float hue, float saturation, float brightness)
		: PostProcessingEffect("renderer/postprocessing/colorcorrection.frag"), 
		r(red), g(green), b(blue), h(hue), s(saturation), v(brightness) {}


	void ColorCorrection::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setFloat("r", r);
		shader.setFloat("g", g);
		shader.setFloat("b", b);
		shader.setFloat("h", h);
		shader.setFloat("s", s);
		shader.setFloat("v", v);
	}
}