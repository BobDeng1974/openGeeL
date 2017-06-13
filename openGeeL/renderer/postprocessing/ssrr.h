#ifndef SSRR_H
#define SSRR_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	//Screen Space Raycasted Reflections post effect
	class SSRR : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		SSRR(unsigned int stepCount = 60, float stepSize = 0.2f, float stepSizeGain = 1.02f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		unsigned int getStepCount() const;
		void setStepCount(unsigned int steps);

		float getStepSize() const;
		void setStepSize(float size);

		float getStepSizeGain() const;
		void setStepSizeGain(float gain);

	protected:
		SSRR(std::string fragmentPath, int sampleCount = 60, float sampleSize = 0.2f, float sampleSizeGain = 1.02f);
		virtual void bindValues();

	private:
		unsigned int steps;
		float stepSize, stepGain;
		ShaderLocation projectionLocation;

	};


	//Multisampled raycasted reflections as post effect that computes
	//indirect specular BRDF component in screen space
	class MultisampledSSRR : public SSRR {

	public:
		MultisampledSSRR(unsigned int sampleCount = 25, unsigned int stepCount = 30, 
			float stepSize = 0.2f, float stepSizeGain = 1.04f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int steps);

	private:
		unsigned int samples;

	};
}

#endif

