#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/simpletexture.h"
#include "../utility/framebuffer.h"
#include "postprocessing.h"
#include "../utility/worldinformation.h"

namespace geeL {

	class BilateralFilter;
	class Camera;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public PostProcessingEffect, public WorldInformationRequester {

	public:
		SSAO(const Camera& camera, PostProcessingEffect& blur, float radius = 5.f);

		virtual void init(ScreenQuad& screen, const FrameBufferInformation& info);
		virtual void draw();

		virtual void addWorldInformation(std::map<WorldMaps, unsigned int> maps,
			std::map<WorldMatrices, const glm::mat4*> matrices,
			std::map<WorldVectors, const glm::vec3*> vectors);

		float getRadius() const;
		void setRadius(float radius);

	protected:
		virtual void bindValues();

	private:
		bool ssaoPass = false;
		float radius;
		unsigned int sampleCount = 64;
		SimpleTexture noiseTexture;
		std::vector<glm::vec3> kernel;
		std::vector<glm::vec3> noise;
		const Camera& camera;
		PostProcessingEffect& blur;
		FrameBuffer tempBuffer;
		const FrameBufferInformation* screenInfo;
	};
}

#endif