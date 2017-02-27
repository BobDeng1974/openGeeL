#ifndef  DOF_H
#define  DOF_H

#include "../utility/framebuffer.h"
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

	protected:
		virtual void bindValues();

	private:
		float threshold;

	};



	//Depth of field post effect that uses image blurring to achieve effect.
	//Cheap but not realistic since circe of confusion is neglected
	class DepthOfFieldBlurred : public PostProcessingEffect, public WorldInformationRequester {

	public:
		DepthOfFieldBlurred(DepthOfFieldBlur& blur,
			const float& focalLength, float aperture = 10.f, float farDistance = 100.f, float blurResolution = 1.f);

		~DepthOfFieldBlurred();

		virtual void init(ScreenQuad& screen);

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors);

	protected:
		virtual void bindValues();

	private:
		const float& focalLength;
		float aperture;
		float farDistance;
		float blurResolution;

		DepthOfFieldBlur& blur;
		FrameBuffer blurBuffer;
		ScreenQuad* blurScreen;

	};
}

#endif
