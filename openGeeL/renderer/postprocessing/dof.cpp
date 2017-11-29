#include "gaussianblur.h"
#include "texturing/texture.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "framebuffer/framebuffer.h"
#include "primitives/screenquad.h"
#include "dof.h"

using namespace std;

namespace geeL {

	DepthOfFieldBlur::DepthOfFieldBlur(float threshold, float sigma)
		: GaussianBlurBase("shaders/postprocessing/dofblur.frag", sigma)
		, threshold(threshold) {

		setKernelsize(9);
	}

	void DepthOfFieldBlur::bindValues() {
		shader.bind<float>("threshold", threshold);

		GaussianBlurBase::bindValues();
	}

	float DepthOfFieldBlur::getThreshold() const {
		return threshold;
	}

	void DepthOfFieldBlur::setThreshold(float value) {
		if (threshold != value && value >= 0.f && value <= 1.f) {
			threshold = value;

			shader.bind<float>("threshold", threshold);
		}
	}

	void  DepthOfFieldBlur::setFocalLength(float value) {
		shader.bind<float>("focalDistance", value);
	}

	void DepthOfFieldBlur::setAperture(float value) {
		shader.bind<float>("aperture", value);
	}

	void DepthOfFieldBlur::setFarDistance(float distance) {
		shader.bind<float>("farDistance", distance);
	}


	DepthOfFieldBlurred::DepthOfFieldBlurred(DepthOfFieldBlur& blur,
		const float focalLength, 
		float aperture, 
		float farDistance, 
		const ResolutionPreset& blurResolution)
			: PostProcessingEffectFS("shaders/postprocessing/dof.frag")
			, blur(blur)
			, blurResolution(blurResolution) {
	
		focalLocation = shader.getLocation("focalDistance");

		setAperture(aperture);
		setFocalLength(focalLength);
		setFarDistance(farDistance);	
	}

	DepthOfFieldBlurred::~DepthOfFieldBlurred() {
		if (blurTexture != nullptr) delete blurTexture;
	}


	void DepthOfFieldBlurred::setImage(const Texture& texture) {
		PostProcessingEffectFS::setImage(texture);

		blur.setImage(texture);
	}

	void DepthOfFieldBlurred::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPositionRoughness(), "gPositionDepth");
		blur.addTextureSampler(provider->requestPositionRoughness(), "gPositionDepth");

		Resolution blurRes = Resolution(parentBuffer->getResolution(), blurResolution);
		if (blurTexture == nullptr)
			blurTexture = new RenderTexture(blurRes, ColorType::RGB16,
				WrapMode::ClampEdge, FilterMode::Linear);
		else
			blurTexture->resize(blurRes);

		blur.init(PostProcessingParameter(parameter, blurRes));
		addTextureSampler(*blurTexture, "blurredImage");
	}

	void DepthOfFieldBlurred::bindValues() {
		blur.bindValues();
	}

	void DepthOfFieldBlurred::drawSubImages() {
		parentBuffer->add(*blurTexture);
		parentBuffer->fill(blur, clearColor);
	}


	void DepthOfFieldBlurred::resizeBlurResolution(const ResolutionPreset& blurResolution) {
		this->blurResolution = blurResolution;

		Resolution newRes = Resolution(resolution, blurResolution);
		blur.setResolution(newRes);
		blurTexture->resize(newRes);
	}

	const ResolutionPreset& DepthOfFieldBlurred::getBlurResolution() const {
		return blurResolution;
	}

	float DepthOfFieldBlurred::getAperture() const {
		return aperture;
	}

	void DepthOfFieldBlurred::setAperture(float aperture) {
		if (aperture > 0.f && aperture != this->aperture) {
			this->aperture = aperture;

			shader.bind<float>("aperture", aperture);
			blur.setAperture(aperture);
		}
	}

	float DepthOfFieldBlurred::getFocalLength() const {
		return focalLength;
	}

	void DepthOfFieldBlurred::setFocalLength(float length) {
		if (length > 0.f && length != focalLength) {
			focalLength = length;

			shader.bind<float>(focalLocation, length);
			blur.setFocalLength(length);
		}

	}

	float DepthOfFieldBlurred::getFarDistance() const {
		return farDistance;
	}

	void DepthOfFieldBlurred::setFarDistance(float distance) {
		if (distance > 0.f && distance != farDistance) {
			farDistance = distance;

			shader.bind<float>("farDistance", distance);
			blur.setFarDistance(distance);
		}
	}

	DepthOfFieldBlur & DepthOfFieldBlurred::getBlur() {
		return blur;
	}

}