#ifndef LENSFLARE_H
#define LENSFLARE_H

#include <vec3.hpp>
#include "../utility/worldinformation.h"
#include "blurredeffect.h"
#include "brightnessfilter.h"

namespace geeL {

	class GaussianBlurBase;
	class ImageTexture;


	class LensFlare : public PostProcessingEffect, public CameraRequester {

	public:
		LensFlare(BlurredPostEffect& filter, float scale = 0.5f, float samples = 4.f, float resolution = 1.f);

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, const ColorBuffer& buffer);

		float getStrength() const;
		float getScale() const;
		float getMaxSamples() const;
		const glm::vec3& getDistortion() const;

		void setScale(float value);
		void setStrength(float value);
		void setMaxSamples(float value);
		void setDistortion(const glm::vec3& value);

		void setStarburstTexture(const ImageTexture& texture);
		void setDirtTexture(const ImageTexture& texture);

	protected:
		virtual void bindValues();

	private:
		glm::vec3 distortion;
		float strength, scale, samples, resolution;
		BlurredPostEffect& filter;
		ColorBuffer filterBuffer;

	};


}

#endif
