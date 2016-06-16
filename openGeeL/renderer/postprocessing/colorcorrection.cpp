#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "colorcorrection.h"

namespace geeL {

	ColorCorrection::ColorCorrection(float red, float green, float blue, float hue, float saturation, float brightness)
		: PostProcessingEffect("renderer/postprocessing/colorcorrection.frag"), 
		r(red), g(green), b(blue), h(hue), s(saturation), v(brightness) {}


	void ColorCorrection::draw() {

		shader.use();
		glUniform1f(glGetUniformLocation(shader.program, "r"), r);
		glUniform1f(glGetUniformLocation(shader.program, "g"), g);
		glUniform1f(glGetUniformLocation(shader.program, "b"), b);
		glUniform1f(glGetUniformLocation(shader.program, "h"), h);
		glUniform1f(glGetUniformLocation(shader.program, "s"), s);
		glUniform1f(glGetUniformLocation(shader.program, "v"), v);
	}
}