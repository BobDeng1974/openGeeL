#ifndef  DOF_H
#define  DOF_H

#include "postprocessing.h"
#include "utility/worldinformation.h"
#include "gaussianblur.h"

namespace geeL {

	class RenderTexture;
	class ScreenQuad;

	//Special gaussian blur that takes distances between pixels into account
	class DepthOfFieldBlur : public GaussianBlurBase {

	public:
		DepthOfFieldBlur(float threshold = 0.1f, float sigma = 2.f);

		virtual void bindValues();

		float getThreshold() const;
		void  setThreshold(float value);

		void setFocalLength(float value);
		void setAperture(float value);
		void setFarDistance(float distance);

	private:
		float threshold;

	};


	//Depth of field post effect that uses image blurring to achieve effect.
	//Cheap but not realistic since circe of confusion is neglected
	class DepthOfFieldBlurred : public PostProcessingEffectFS {

	public:
		DepthOfFieldBlurred(DepthOfFieldBlur& blur, 
			const float focalLength, 
			float aperture = 10.f, 
			float farDistance = 100.f, 
			const ResolutionPreset& resolution = ResolutionPreset::FULLSCREEN);

		virtual ~DepthOfFieldBlurred();

		virtual void setImage(const ITexture& texture);
		virtual void init(const PostProcessingParameter& parameter);
		virtual void bindValues();

		void resizeBlurResolution(const ResolutionPreset& blurResolution);
		const ResolutionPreset& getBlurResolution() const;

		float getAperture() const;
		void setAperture(float aperture);

		float getFocalLength() const;
		void setFocalLength(float length);

		float getFarDistance() const;
		void setFarDistance(float distance);

		DepthOfFieldBlur& getBlur();

		virtual std::string toString() const;

		virtual void drawSubImages();

	private:
		float focalLength;
		float aperture;
		float farDistance;
		ResolutionPreset blurResolution;

		ShaderLocation focalLocation;

		DepthOfFieldBlur& blur;
		RenderTexture* blurTexture = nullptr;

	};


	inline std::string DepthOfFieldBlurred::toString() const {
		return "Depth of Field";
	}

}

#endif
