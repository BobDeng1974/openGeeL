#ifndef COLORCORRECTION_H
#define COLORCORRECTION_H

#include "postprocessing.h"

namespace geeL {

	class ColorCorrection : public PostProcessingEffect {

	public:
		ColorCorrection(float red = 1.f, float green = 1.f, float blue = 1.f, 
			float hue = 1.f, float saturation = 1.f, float brightness = 1.f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		float getRed() const;
		float getGreen() const;
		float getBlue() const;
		float getHue() const;
		float getSaturation() const;
		float getBrightness() const;

		void setRed(float value);
		void setGreen(float value);
		void setBlue(float value);
		void setHue(float value);
		void setSaturation(float value);
		void setBrightness(float value);

	protected:
		float r, g, b, h, s, v;

	};
}

#endif