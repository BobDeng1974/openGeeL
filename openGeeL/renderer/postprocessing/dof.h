#ifndef  DOF_H
#define  DOF_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"
#include "gaussianblur.h"

namespace geeL {

	class RenderTexture;
	class ScreenQuad;

	//Special gaussian blur that takes distances between pixels into account
	class DepthOfFieldBlur : public GaussianBlurBase {

	public:
		DepthOfFieldBlur(float threshold = 0.1f, float sigma = 2.f);

		void bindDoFData(float focalLength, float aperture, float farDistance);

		float getThreshold() const;
		void  setThreshold(float value);

		void setFocalLength(float value);
		void setAperture(float value);

	protected:
		virtual void bindValues();

	private:
		float threshold;

	};


	//Depth of field post effect that uses image blurring to achieve effect.
	//Cheap but not realistic since circe of confusion is neglected
	class DepthOfFieldBlurred : public PostProcessingEffect, public WorldMapRequester {

	public:
		DepthOfFieldBlurred(DepthOfFieldBlur& blur, const float& focalLength, float aperture = 10.f, 
			float farDistance = 100.f, ResolutionScale blurResolution = FULLSCREEN);
		~DepthOfFieldBlurred();

		virtual void setImageBuffer(const Texture& texture);
		virtual void init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution);

		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		void resizeBlurResolution(ResolutionScale blurResolution);
		const ResolutionScale& getBlurResolution() const;

		float getAperture() const;
		void setAperture(float aperture);

		DepthOfFieldBlur& getBlur();

	protected:
		virtual void bindValues();

	private:
		const float& focalLength;
		float aperture;
		float farDistance;
		ResolutionScale blurResolution;

		ShaderLocation focalLocation;

		DepthOfFieldBlur& blur;
		RenderTexture* blurTexture = nullptr;

	};
}

#endif
