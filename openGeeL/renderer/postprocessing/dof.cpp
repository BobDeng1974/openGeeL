#include "gaussianblur.h"
#include "../utility/screenquad.h"
#include "dof.h"

using namespace std;

namespace geeL {

	DepthOfFieldBlur::DepthOfFieldBlur(unsigned int strength, float threshold)
		: GaussianBlur(strength, "renderer/postprocessing/dofblur.frag"), threshold(threshold) {}

	void DepthOfFieldBlur::bindValues() {
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setFloat("threshold", threshold);

		GaussianBlur::bindValues();
	}

	void DepthOfFieldBlur::bindDoFData(float focalLength, float aperture, float farDistance) {
		shader.use();

		shader.setFloat("focalDistance", focalLength);
		shader.setFloat("aperture", aperture);
		shader.setFloat("farDistance", farDistance);
	}



	DepthOfFieldBlurred::DepthOfFieldBlurred(DepthOfFieldBlur& blur,
		const float& focalLength, float aperture, float farDistance, float blurResolution)
			: PostProcessingEffect("renderer/postprocessing/dof.frag"), 
				blur(blur), focalLength(focalLength), aperture(aperture), 
				farDistance(farDistance), blurResolution(blurResolution), blurScreen(nullptr) {}

	DepthOfFieldBlurred::~DepthOfFieldBlurred() {
		if(blurScreen != nullptr)
			delete blurScreen;
	}


	void DepthOfFieldBlurred::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		blurScreen = new ScreenQuad(screen.width * blurResolution, screen.height * blurResolution);
		blurScreen->init();

		blurBuffer.init(blurScreen->width, blurScreen->height);
		blur.init(*blurScreen);
		buffers.push_back(blurBuffer.getColorID());
	}

	void DepthOfFieldBlurred::bindValues() {
		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
		shader.setInteger("blurredImage", shader.mapOffset + 2);

		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;

		shader.setFloat("focalDistance", dist);
		shader.setFloat("aperture", aperture);
		shader.setFloat("farDistance", farDistance);

		blur.bindDoFData(dist, aperture, farDistance);
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screen->width, screen->height);
		FrameBuffer::bind(parentFBO);
	}

	void DepthOfFieldBlurred::addWorldInformation(map<WorldMaps, unsigned int> maps,
		map<WorldMatrices, const glm::mat4*> matrices,
		map<WorldVectors, const glm::vec3*> vectors) {

		addBuffer( {maps[WorldMaps::PositionDepth]} );
		blur.setBuffer({ buffers.front(), *next(buffers.begin()) });
	}
}