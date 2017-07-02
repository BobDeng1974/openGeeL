#ifndef  DOF_H
#define  DOF_H

#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"
#include "../utility/worldinformation.h"
#include "gaussianblur.h"

namespace geeL {

	class ScreenQuad;

	//Special gaussian blur that takes distances between pixels into account
	class DepthOfFieldBlur : public GaussianBlur {

	public:
		DepthOfFieldBlur(unsigned int strength = 1, float threshold = 0.1f);

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
		DepthOfFieldBlurred(DepthOfFieldBlur& blur,
			const float& focalLength, float aperture = 10.f, float farDistance = 100.f, float blurResolution = 1.f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		void resizeBlurResolution(float blurResolution);
		float getBlurResolution() const;

		float getAperture() const;
		void setAperture(float aperture);

		float getBlurThreshold() const;
		void  setBlurThreshold(float value);

	protected:
		virtual void bindValues();

	private:
		const float& focalLength;
		float aperture;
		float farDistance;
		float blurResolution;

		ShaderLocation focalLocation;

		DepthOfFieldBlur& blur;
		ColorBuffer blurBuffer;
		const FrameBufferInformation* screenInfo;

	};
}

#endif
