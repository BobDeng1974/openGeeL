#ifndef  DOF_H
#define  DOF_H

#include "../utility/framebuffer.h"
#include "worldpostprocessing.h"
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
	class DepthOfFieldBlurred : public WorldPostProcessingEffect {

	public:
		DepthOfFieldBlurred(DepthOfFieldBlur& blur,
			float focalLength = 5.f, float aperture = 10.f, float farDistance = 100.f, float blurResolution = 1.f);

		~DepthOfFieldBlurred();

		virtual void setScreen(ScreenQuad& screen);

		virtual WorldMaps requiredWorldMaps() const;
		virtual WorldMatrices requiredWorldMatrices() const;
		virtual WorldVectors requiredWorldVectors() const;
		virtual std::list<WorldMaps> requiredWorldMapsList() const;

		virtual void addWorldInformation(std::list<unsigned int> maps,
			std::list<glm::mat4> matrices, std::list<glm::vec3> vectors);

	protected:
		virtual void bindValues();

	private:
		float focalLength;
		float aperture;
		float farDistance;
		float blurResolution;

		DepthOfFieldBlur& blur;
		FrameBuffer blurBuffer;
		ScreenQuad* blurScreen;

	};
}

#endif
