#include "gaussianblur.h"
#include "../primitives/screenquad.h"
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

	void  DepthOfFieldBlur::setFocalLength(float value) {
		shader.use();
		shader.setFloat("focalDistance", value);
	}

	void DepthOfFieldBlur::setAperture(float value) {
		shader.use();
		shader.setFloat("aperture", value);
	}


	DepthOfFieldBlurred::DepthOfFieldBlurred(DepthOfFieldBlur& blur,
		const float& focalLength, float aperture, float farDistance, float blurResolution)
			: PostProcessingEffect("renderer/postprocessing/dof.frag"), 
				blur(blur), focalLength(focalLength), aperture(aperture), 
				farDistance(farDistance), blurResolution(blurResolution) {}


	void DepthOfFieldBlurred::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		shader.setFloat("farDistance", farDistance);
		shader.setFloat("aperture", aperture);

		focalLocation = shader.getLocation("focalDistance");

		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;
		blur.bindDoFData(dist, aperture, farDistance);

		screenInfo = &info;
		blurBuffer.init(int(info.width * blurResolution), int(info.height * blurResolution), 
			ColorType::RGB16, FilterMode::Linear);

		blur.init(screen, blurBuffer.info);
		addBuffer(blurBuffer.getTexture(), "blurredImage");
	}

	void DepthOfFieldBlurred::bindValues() {
		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;

		shader.setFloat(focalLocation, dist);
		blur.setFocalLength(dist);
		
		blurBuffer.fill(blur);

		FrameBuffer::resetSize(screenInfo->width, screenInfo->height);
		FrameBuffer::bind(parentFBO);
	}

	void DepthOfFieldBlurred::draw() {
		shader.use();
		bindValues();

		//Switch shader again since shader of encapsuled 
		//dof blur was set active during 'bindValues'
		shader.use();
		bindToScreen();
	}

	void DepthOfFieldBlurred::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		blur.addBuffer(*maps[WorldMaps::PositionDepth], "gPositionDepth");
		blur.setBuffer(getBuffer());
	}

	void DepthOfFieldBlurred::resizeBlurResolution(float blurResolution) {
		if (blurResolution > 0.f && blurResolution < 1.f) {
			this->blurResolution = blurResolution;

			blurBuffer.resize(int(screenInfo->width * blurResolution),
				int(screenInfo->height * blurResolution));
		}
	}

	float DepthOfFieldBlurred::getBlurResolution() const {
		return blurResolution;
	}

	float DepthOfFieldBlurred::getAperture() const {
		return aperture;
	}

	void DepthOfFieldBlurred::setAperture(float aperture) {
		if (aperture > 0.f && aperture != this->aperture) {
			this->aperture = aperture;

			shader.use();
			shader.setFloat("aperture", aperture);
			blur.setAperture(aperture);
		}
	}
}