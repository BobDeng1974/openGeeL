#define GLEW_STATIC
#include <glew.h>
#include <glm.hpp>
#include "shader/rendershader.h"
#include "colorcorrection.h"

namespace geeL {

	ColorCorrection::ColorCorrection(float red, float green, float blue,
		float hue, float saturation, float brightness, float contrast, float vibrance)
			: PostProcessingEffectFS("shaders/postprocessing/colorcorrection.frag")
			, r(red), g(green), b(blue)
			, h(hue), s(saturation), v(brightness)
			, c(contrast), vi(vibrance) {}


	void ColorCorrection::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<float>("r", r);
		shader.bind<float>("g", g);
		shader.bind<float>("b", b);
		shader.bind<float>("h", h);
		shader.bind<float>("s", s);
		shader.bind<float>("v", v);
		shader.bind<float>("c", c);
		shader.bind<float>("vi", vi);

		shader.bind<glm::vec2>("direction", distortionDirection);
		shader.bind<glm::vec3>("distortion", distortion);
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

	float ColorCorrection::getContrast() const {
		return c;
	}

	float ColorCorrection::getVibrance() const {
		return vi;
	}

	const glm::vec2& ColorCorrection::getDistortionDirection() const {
		return distortionDirection;
	}

	const glm::vec3& ColorCorrection::getChromaticDistortion() const {
		return distortion;
	}

	void ColorCorrection::setRed(float value) {
		if (r != value && value >= 0.f && value <= 1.f) {
			r = value;

			shader.bind<float>("r", r);
		}
	}

	void ColorCorrection::setGreen(float value) {
		if (g != value && value >= 0.f && value <= 1.f) {
			g = value;

			shader.bind<float>("g", g);
		}
	}

	void ColorCorrection::setBlue(float value) {
		if (b != value && value >= 0.f && value <= 1.f) {
			b = value;

			shader.bind<float>("b", b);
		}
	}

	void ColorCorrection::setHue(float value) {
		if (h != value && value >= 0.f && value <= 1.f) {
			h = value;

			shader.bind<float>("h", h);
		}
	}

	void ColorCorrection::setSaturation(float value) {
		if (s != value && value >= 0.f && value <= 1.f) {
			s = value;

			shader.bind<float>("s", s);
		}
	}

	void ColorCorrection::setBrightness(float value) {
		if (v != value && value >= 0.f && value <= 1.f) {
			v = value;

			shader.bind<float>("v", v);
		}
	}

	void ColorCorrection::setContrast(float value) {
		if (c != value && value >= 0.f) {
			c = value;

			shader.bind<float>("c", c);
		}
	}

	void ColorCorrection::setVibrance(float value) {
		if (vi != value && value >= -2.f && value <= 2.f) {
			vi = value;

			shader.bind<float>("vi", vi);
		}
	}

	void ColorCorrection::setDistortionDirection(const glm::vec2& value) {
		if (distortionDirection != value) {
			distortionDirection = glm::normalize(value);

			shader.bind<glm::vec2>("direction", distortionDirection);
		}
	}

	void ColorCorrection::setChromaticDistortion(const glm::vec3& value) {
		if (distortion != value) {
			distortion = value;

			shader.bind<glm::vec3>("distortion", distortion);
		}
	}

}