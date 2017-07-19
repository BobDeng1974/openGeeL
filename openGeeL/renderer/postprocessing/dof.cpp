#include "gaussianblur.h"
#include "../texturing/texture.h"
#include "../texturing/rendertexture.h"
#include "../framebuffer/framebuffer.h"
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

	DepthOfFieldBlurred::~DepthOfFieldBlurred() {
		if (blurTexture != nullptr) delete blurTexture;
	}


	void DepthOfFieldBlurred::setImageBuffer(const Texture& texture) {
		PostProcessingEffect::setImageBuffer(texture);

		blur.setImageBuffer(texture);
	}

	void DepthOfFieldBlurred::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffect::init(screen, buffer, resolution);

		shader.setFloat("farDistance", farDistance);
		shader.setFloat("aperture", aperture);

		focalLocation = shader.getLocation("focalDistance");

		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;
		blur.bindDoFData(dist, aperture, farDistance);

		Resolution blurRes = Resolution(parentBuffer->getResolution(), blurResolution);
		blurTexture = new RenderTexture(blurRes, ColorType::RGB16, 
			WrapMode::ClampEdge, FilterMode::Linear);

		blur.init(screen, buffer, blurRes);
		addImageBuffer(*blurTexture, "blurredImage");
	}

	void DepthOfFieldBlurred::bindValues() {
		//Clamp focal length with reasonable values
		float dist = (focalLength < 0.f || focalLength > 30.f) ? 30.f : focalLength;
		shader.setFloat(focalLocation, dist);

		blur.setFocalLength(dist);
		parentBuffer->add(*blurTexture);
		parentBuffer->fill(blur);

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

	void DepthOfFieldBlurred::resizeBlurResolution(ResolutionScale blurResolution) {
		this->blurResolution = blurResolution;

		Resolution newRes = Resolution(resolution, blurResolution);
		blur.setResolution(newRes);
		blurTexture->resize(newRes);
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