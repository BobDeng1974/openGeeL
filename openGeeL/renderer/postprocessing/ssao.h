#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/simpletexture.h"
#include "../framebuffer/framebuffer.h"
#include "../utility/worldinformation.h"
#include "postprocessing.h"

namespace geeL {

	class BilateralFilter;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public PostProcessingEffect, public WorldMapRequester, public CameraRequester {

	public:
		SSAO(PostProcessingEffect& blur, float radius = 5.f);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps);

		float getRadius() const;
		void setRadius(float radius);

	protected:
		virtual void bindValues();

	private:
		bool ssaoPass = false;
		float radius;
		unsigned int sampleCount = 32;

		SimpleTexture noiseTexture;
		std::vector<glm::vec3> kernel;
		std::vector<glm::vec3> noise;
		PostProcessingEffect& blur;

		ShaderLocation projectionLocation;
		ColorBuffer tempBuffer;
		const FrameBufferInformation* screenInfo;
		
	};
}

#endif