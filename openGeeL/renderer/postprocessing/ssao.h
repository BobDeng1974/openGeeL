#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/simpletexture.h"
#include "../utility/framebuffer.h"
#include "worldpostprocessing.h"

namespace geeL {

	class Camera;
	class SimpleBlur;

	//Screen Space Ambient Occlusion Post Effect
	class SSAO : public WorldPostProcessingEffect {

	public:
		SSAO(const Camera& camera, SimpleBlur& blur, float radius = 5.f);

		virtual void setScreen(ScreenQuad& screen);
		virtual void draw();

		virtual WorldMaps requiredWorldMaps() const;
		virtual WorldMatrices requiredWorldMatrices() const;
		virtual WorldVectors requiredWorldVectors() const;
		virtual std::list<WorldMaps> requiredWorldMapsList() const;

		virtual void addWorldInformation(std::list<unsigned int> maps,
			std::list<glm::mat4> matrices, std::list<glm::vec3> vectors);

	protected:
		virtual void bindValues();

	private:
		bool ssaoPass = false;
		const float radius;
		unsigned int sampleCount = 64;
		SimpleTexture noiseTexture;
		std::vector<glm::vec3> kernel;
		std::vector<glm::vec3> noise;
		const Camera& camera;
		SimpleBlur& blur;
		FrameBuffer tempBuffer;
	};
}

#endif