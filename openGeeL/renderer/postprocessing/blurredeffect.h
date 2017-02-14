#ifndef BLURREDEFFECT_H
#define BLURREDEFFECT_H

#include "../utility/framebuffer.h"
#include "worldpostprocessing.h"
#include "postprocessing.h"

namespace geeL {

	class GaussianBlur;

	//Wrapper effect that can blur and scale another post processing effect
	class BlurredPostEffect : public PostProcessingEffect {

	public:
		BlurredPostEffect(PostProcessingEffect& effect, GaussianBlur& blur, float resolution = 1.f);
		~BlurredPostEffect();

		virtual void setScreen(ScreenQuad& screen);

	protected:
		virtual void bindValues();

	private:
		float resolution;
		PostProcessingEffect& effect;
		GaussianBlur& blur;
		FrameBuffer effectBuffer;
		FrameBuffer blurBuffer;
		ScreenQuad* effectScreen = nullptr;
	};


	//Wrapper effect that can blur and scale another world post processing effect
	class BlurredWorldPostEffect : public WorldPostProcessingEffect {

	public:
		BlurredWorldPostEffect(WorldPostProcessingEffect& effect, GaussianBlur& blur, float resolution = 1.f);
		~BlurredWorldPostEffect();

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
		float resolution;
		WorldPostProcessingEffect& effect;
		GaussianBlur& blur;
		FrameBuffer effectBuffer;
		FrameBuffer blurBuffer;
		ScreenQuad* effectScreen = nullptr;
	};
}

#endif