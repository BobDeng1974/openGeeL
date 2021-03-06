#ifndef LENSFLARE_H
#define LENSFLARE_H

#include <vec3.hpp>
#include "utility/worldinformation.h"
#include "blurredeffect.h"
#include "brightnessfilter.h"

namespace geeL {

	class ImageTexture;
	class RenderTexture;


	class LensFlare : public PostProcessingEffectFS, public CameraRequester {

	public:
		LensFlare(BlurredPostEffect& filter, 
			float scale = 0.5f, 
			float samples = 4.f, 
			const ResolutionPreset& resolution = ResolutionPreset::FULLSCREEN,
			bool useBloom = true);

		virtual ~LensFlare();

		virtual void setImage(const ITexture& texture);
		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

		bool  getBloomUse() const;
		float getStrength() const;
		float getScale() const;
		float getMaxSamples() const;
		const glm::vec3& getDistortion() const;

		void setBloomUse(bool value);
		void setScale(float value);
		void setStrength(float value);
		void setMaxSamples(float value);
		void setDistortion(const glm::vec3& value);

		void setStarburstTexture(const ImageTexture& texture);
		void setDirtTexture(const ImageTexture& texture);

		virtual std::string toString() const;

	protected:
		virtual void drawSubImages();

	private:
		bool useBloom;
		glm::vec3 distortion;
		float strength, scale, samples;
		ResolutionPreset filterResolution;

		BlurredPostEffect& filter;
		RenderTexture* filterTexture = nullptr;

	};



	inline std::string LensFlare::toString() const {
		return "Lens Flare";
	}


}

#endif
