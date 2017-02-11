#include "gaussianblur.h"
#include "../utility/screenquad.h"
#include "dof.h"
#include <iostream>

using namespace std;

namespace geeL {

	DepthOfFieldBlurred::DepthOfFieldBlurred(GaussianBlur& blur, float focalLength, float aperture)
		: WorldPostProcessingEffect("renderer/postprocessing/dof.frag"), 
			blur(blur), focalLength(focalLength), aperture(aperture) {}


	void DepthOfFieldBlurred::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

		blurBuffer.init(screen.width, screen.height);
		blur.setScreen(screen);
		buffers.push_back(blurBuffer.getColorID());
	}

	void DepthOfFieldBlurred::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("blurredImage", shader.mapOffset + 2);

		shader.setFloat("focalDistance", focalLength);
		shader.setFloat("aperture", aperture);

		blur.setBuffer(buffers.front());
		blurBuffer.fill(blur);

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