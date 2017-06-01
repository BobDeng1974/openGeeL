#ifndef SSRR_H
#define SSRR_H

#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	//Screen Space Raycasted Reflections post effect
	class SSRR : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		SSRR(unsigned int sampleCount = 60, float sampleSize = 0.2f, float sampleSizeGain = 1.02f);

		virtual void init(ScreenQuad& screen, const FrameBuffer& buffer);
		virtual void addWorldInformation(std::map<WorldMaps, const Texture*> maps);

		unsigned int getSampleCount() const;
		void setSampleCount(unsigned int samples);

		float getSampleSize() const;
		void setSampleSize(float size);

		float getSampleSizeGain() const;
		void setSampleSizeGain(float gain);

	protected:
		virtual void bindValues();

	private:
		unsigned int samples;
		float sampleSize, sampleGain;
		ShaderLocation projectionLocation;

	};
}

#endif

