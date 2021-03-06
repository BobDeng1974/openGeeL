#ifndef COLORCORRECTION_H
#define COLORCORRECTION_H

#include <vec2.hpp>
#include <vec3.hpp>
#include "postprocessing.h"

namespace geeL {

	class ColorCorrection : public PostProcessingEffectFS {

	public:
		ColorCorrection(float red = 1.f, float green = 1.f, float blue = 1.f, 
			float hue = 1.f, float saturation = 1.f, float brightness = 1.f, 
			float contrast = 1.f, float vibrance = 0.f);

		virtual void init(const PostProcessingParameter& parameter);

		float getRed() const;
		float getGreen() const;
		float getBlue() const;
		float getHue() const;
		float getSaturation() const;
		float getBrightness() const;
		float getContrast() const;
		float getVibrance() const;

		const glm::vec2& getDistortionDirection() const;
		const glm::vec3& getChromaticDistortion() const;

		void setRed(float value);
		void setGreen(float value);
		void setBlue(float value);
		void setHue(float value);
		void setSaturation(float value);
		void setBrightness(float value);
		void setContrast(float value);
		void setVibrance(float value);

		void setDistortionDirection(const glm::vec2& value);
		void setChromaticDistortion(const glm::vec3& value);

		virtual std::string toString() const;

	protected:
		glm::vec2 distortionDirection;
		glm::vec3 distortion;
		float r, g, b, h, s, v, c, vi;

	};


	inline std::string ColorCorrection::toString() const {
		return "Color Correction";
	}

}

#endif