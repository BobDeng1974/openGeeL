#include "../utility/screenquad.h"
#include "gaussianblur.h"
#include "blurredeffect.h"

using namespace std;

namespace geeL {

	BlurredPostEffect::BlurredPostEffect(PostProcessingEffect& effect, GaussianBlur& blur, float resolution)
		: PostProcessingEffect("renderer/postprocessing/combine.frag"),
		effect(effect), blur(blur), resolution(resolution) {

		effect.effectOnly(true);
	}

	BlurredPostEffect::~BlurredPostEffect() {
		if (effectScreen != nullptr)
			delete effectScreen;
	}


	void BlurredPostEffect::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		effectScreen = new ScreenQuad(screen.width * resolution, screen.height * resolution);
		effectScreen->init();

		effectBuffer.init(effectScreen->width, effectScreen->height);
		blurBuffer.init(effectScreen->width, effectScreen->height);

		effect.setScreen(*effectScreen);
		blur.setScreen(*effectScreen);
		blur.setParentFBO(blurBuffer.fbo);

		buffers.push_back(blurBuffer.getColorID());
	}


	void BlurredPostEffect::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);

		effect.setBuffer(buffers.front());
		effectBuffer.fill(effect);

		blur.setBuffer(effectBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}



	BlurredWorldPostEffect::BlurredWorldPostEffect(WorldPostProcessingEffect& effect, GaussianBlur& blur, float resolution)
		: WorldPostProcessingEffect("renderer/postprocessing/combine.frag"),
		effect(effect), blur(blur), resolution(resolution) {

		effect.effectOnly(true);
	}

	BlurredWorldPostEffect::~BlurredWorldPostEffect() {
		if (effectScreen != nullptr)
			delete effectScreen;
	}


	void BlurredWorldPostEffect::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		effectScreen = new ScreenQuad(screen.width * resolution, screen.height * resolution);
		effectScreen->init();

		effectBuffer.init(effectScreen->width, effectScreen->height);
		blurBuffer.init(effectScreen->width, effectScreen->height);

		effect.setScreen(*effectScreen);
		blur.setScreen(*effectScreen);
		blur.setParentFBO(blurBuffer.fbo);

		buffers.push_back(blurBuffer.getColorID());
	}


	void BlurredWorldPostEffect::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("image2", shader.mapOffset + 1);

		effectBuffer.fill(effect);

		blur.setBuffer(effectBuffer.getColorID());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}


	WorldMaps BlurredWorldPostEffect::requiredWorldMaps() const {
		return effect.requiredWorldMaps();
	}

	WorldMatrices BlurredWorldPostEffect::requiredWorldMatrices() const {
		return effect.requiredWorldMatrices();
	}

	WorldVectors BlurredWorldPostEffect::requiredWorldVectors() const {
		return effect.requiredWorldVectors();
	}

	list<WorldMaps> BlurredWorldPostEffect::requiredWorldMapsList() const {
		return effect.requiredWorldMapsList();
	}

	void BlurredWorldPostEffect::addWorldInformation(list<unsigned int> maps,
		list<glm::mat4> matrices, list<glm::vec3> vectors) {

		//Set own buffer to first map because it is assumed that 
		//it is the 'main' one, e.g. the rendered image
		setBuffer(maps.front());
		effect.addWorldInformation(maps, matrices, vectors);
	}

}