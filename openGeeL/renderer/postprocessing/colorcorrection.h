#ifndef COLORCORRECTION_H
#define COLORCORRECTION_H

#include <vec2.hpp>
#include <vec3.hpp>
#include "postprocessing.h"

namespace geeL {

	class ColorCorrection : public PostProcessingEffect {

	public:
		ColorCorrection(float red = 1.f, float green = 1.f, float blue = 1.f, 
			float hue = 1.f, float saturation = 1.f, float brightness = 1.f);

		virtual void init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution);

		float getRed() const;
		float getGreen() const;
		float getBlue() const;
		float getHue() const;
		float getSaturation() const;
		float getBrightness() const;

		const glm::vec2& getDistortionDirection() const;
		const glm::vec3& getChromaticDistortion() const;

		void setRed(float value);
		void setGreen(float value);
		void setBlue(float value);
		void setHue(float value);
		void setSaturation(float value);
		void setBrightness(float value);

		void setDistortionDirection(const glm::vec2& value);
		void setChromaticDistortion(const glm::vec3& value);

	protected:
		glm::vec2 distortionDirection;
		glm::vec3 distortion;
		float r, g, b, h, s, v;

	};
}

#endif