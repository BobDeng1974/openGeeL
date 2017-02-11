#include "gaussianblur.h"
#include "../utility/screenquad.h"
#include "dof.h"
#include <iostream>

using namespace std;

namespace geeL {

	DepthOfFieldBlurred::DepthOfFieldBlurred(GaussianBlur& blur, 
		float focalLength, float aperture, float farDistance, float blurResolution)
			: WorldPostProcessingEffect("renderer/postprocessing/dof.frag"), 
				blur(blur), focalLength(focalLength), aperture(aperture), 
				farDistance(farDistance), blurResolution(blurResolution), blurScreen(nullptr) {}

	DepthOfFieldBlurred::~DepthOfFieldBlurred() {
		if(blurScreen != nullptr)
			delete blurScreen;
	}


	void DepthOfFieldBlurred::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		blurScreen = new ScreenQuad(screen.width * blurResolution, screen.height * blurResolution);
		blurScreen->init();

		blurBuffer.init(blurScreen->width, blurScreen->height);
		blur.setScreen(*blurScreen);
		buffers.push_back(blurBuffer.getColorID());
	}

	void DepthOfFieldBlurred::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("blurredImage", shader.mapOffset + 2);

		shader.setFloat("focalDistance", focalLength);
		shader.setFloat("aperture", aperture);
		shader.setFloat("farDistance", farDistance);

		blur.setBuffer(buffers.front());
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}

	WorldMaps DepthOfFieldBlurred::requiredWorldMaps() const {
		return (WorldMaps::RenderedImage | WorldMaps::PositionDepth);
	}

	WorldMatrices DepthOfFieldBlurred::requiredWorldMatrices() const {
		return WorldMatrices::None;
	}

	WorldVectors DepthOfFieldBlurred::requiredWorldVectors() const {
		return WorldVectors::None;
	}

	list<WorldMaps> DepthOfFieldBlurred::requiredWorldMapsList() const {
		return{ WorldMaps::RenderedImage, WorldMaps::PositionDepth };
	}

	void DepthOfFieldBlurred::addWorldInformation(list<unsigned int> maps,
		list<glm::mat4> matrices, list<glm::vec3> vectors) {

		if (maps.size() != 2)
			throw "Wrong number of texture maps attached to SSAO";

		setBuffer(maps);
	}
}