#include "gaussianblur.h"
#include "../texturing/texture.h"
#include "../primitives/screenquad.h"
#include "dof.h"

using namespace std;

namespace geeL {

	DepthOfFieldBlur::DepthOfFieldBlur(float threshold, float sigma)
		: GaussianBlurBase("renderer/postprocessing/dofblur.frag", sigma), threshold(threshold) {

		setKernelsize(9);
	}

	void DepthOfFieldBlur::bindValues() {
		shader.setFloat("threshold", threshold);

		GaussianBlurBase::bindValues();
	}

	void DepthOfFieldBlur::bindDoFData(float focalLength, float aperture, float farDistance) {
		shader.use();

		shader.setFloat("focalDistance", focalLength);
		shader.setFloat("aperture", aperture);
		shader.setFloat("farDistance", farDistance);
	}

	float DepthOfFieldBlur::getThreshold() const {
		return threshold;
	}

	void DepthOfFieldBlur::setThreshold(float value) {
		if (threshold != value && value >= 0.f && value <= 1.f) {
			threshold = value;

			shader.use();
			shader.setFloat("threshold", threshold);
		}
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
		const float& focalLength, float aperture, float farDistance, ResolutionScale blurResolution)
			: PostProcessingEffect("renderer/postprocessing/dof.frag"), 
				blur(blur), focalLength(focalLength), aperture(aperture), 
				farDistance(farDistance), blurResolution(blurResolution) {}


	void DepthOfFieldBlurred::setImageBuffer(const Texture& texture) {
		PostProcessingEffect::setImageBuffer(texture);

		blur.setImageBuffer(texture);
	}

	void DepthOfFieldBlurred::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		shader.setFloat("farDistance", farDistance);
		shader.setFloat("aperture", aperture);

		focalLocation = shader.getLocation("focalDistance");

		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;
		blur.bindDoFData(dist, aperture, farDistance);

		blurBuffer.init(int(parentBuffer->getWidth() * blurResolution), int(parentBuffer->getHeight() * blurResolution),
			ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);

		blur.init(screen, blurBuffer);
		addImageBuffer(blurBuffer.getTexture(), "blurredImage");
	}

	void DepthOfFieldBlurred::bindValues() {
		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;

		shader.setFloat(focalLocation, dist);
		blur.setFocalLength(dist);
		
		blurBuffer.fill(blur);

		parentBuffer->resetSize();
		parentBuffer->bind();
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
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
		blur.addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
	}

	void DepthOfFieldBlurred::resizeBlurResolution(float blurResolution) {
		if (blurResolution > 0.f && blurResolution < 1.f) {
			this->blurResolution = blurResolution;

			blurBuffer.resize(blurResolution);
		}
	}

	const ResolutionScale& DepthOfFieldBlurred::getBlurResolution() const {
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

	DepthOfFieldBlur & DepthOfFieldBlurred::getBlur() {
		return blur;
	}

}