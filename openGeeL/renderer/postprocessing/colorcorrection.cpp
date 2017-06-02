#define GLEW_STATIC
#include <glew.h>
#include "../shader/rendershader.h"
#include "colorcorrection.h"

namespace geeL {

	ColorCorrection::ColorCorrection(float red, float green, float blue, float hue, float saturation, float brightness)
		: PostProcessingEffect("renderer/postprocessing/colorcorrection.frag"), 
		r(red), g(green), b(blue), h(hue), s(saturation), v(brightness) {}


	void ColorCorrection::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("r", r);
		shader.setFloat("g", g);
		shader.setFloat("b", b);
		shader.setFloat("h", h);
		shader.setFloat("s", s);
		shader.setFloat("v", v);
	}


	float ColorCorrection::getRed() const {
		return r;
	}

	float ColorCorrection::getGreen() const {
		return g;
	}

	float ColorCorrection::getBlue() const {
		return b;
	}

	float ColorCorrection::getHue() const {
		return h;
	}

	float ColorCorrection::getSaturation() const {
		return s;
	}

	float ColorCorrection::getBrightness() const {
		return v;
	}

	void ColorCorrection::setRed(float value) {
		if (r != value && value >= 0.f && value <= 1.f) {
			r = value;

			shader.use();
			shader.setFloat("r", r);
		}
	}

	void ColorCorrection::setGreen(float value) {
		if (g != value && value >= 0.f && value <= 1.f) {
			g = value;

			shader.use();
			shader.setFloat("g", g);
		}
	}

	void ColorCorrection::setBlue(float value) {
		if (b != value && value >= 0.f && value <= 1.f) {
			b = value;

			shader.use();
			shader.setFloat("b", b);
		}
	}

	void ColorCorrection::setHue(float value) {
		if (h != value && value >= 0.f && value <= 1.f) {
			h = value;

			shader.use();
			shader.setFloat("h", h);
		}
	}

	void ColorCorrection::setSaturation(float value) {
		if (s != value && value >= 0.f && value <= 1.f) {
			s = value;

			shader.use();
			shader.setFloat("s", s);
		}
	}

	void ColorCorrection::setBrightness(float value) {
		if (v != value && value >= 0.f && value <= 1.f) {
			v = value;

			shader.use();
			shader.setFloat("v", v);
		}
	}

}