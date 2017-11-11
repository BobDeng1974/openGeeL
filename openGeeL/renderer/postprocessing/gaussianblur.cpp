#define GLEW_STATIC
#include <glew.h>
#include "texturing/texture.h"
#include "shader/rendershader.h"
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "sobel.h"
#include "gaussianblur.h"
#include <iostream>

const double PI = 3.14159265359;

using namespace glm;
using namespace std;

namespace geeL {

	GaussianBlurBase::GaussianBlurBase(float sigma)
		: GaussianBlurBase("renderer/postprocessing/gaussianblur1.frag", sigma) {}

	GaussianBlurBase::GaussianBlurBase(string shaderPath, float sigma)
		: PostProcessingEffectFS(shaderPath), mainBuffer(nullptr), sigma(sigma) {

		updateKernel();
	}

	GaussianBlurBase::~GaussianBlurBase() {
		if (tempTexture != nullptr) delete tempTexture;
	}

	void GaussianBlurBase::setImage(const Texture& texture) {
		PostProcessingEffectFS::setImage(texture);

		mainBuffer = &texture;
	}

	void GaussianBlurBase::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		tempTexture = new RenderTexture(resolution, ColorType::RGB16, 
			WrapMode::ClampEdge, FilterMode::Linear);

		horLocation = shader.getLocation("horizontal");
	}

	void GaussianBlurBase::drawSubImages() {
		//1. Fill the temporary buffer with the result of the horizontal blurring
		shader.bind<int>(horLocation, true);

		if (mainBuffer != nullptr)
			addTextureSampler(*mainBuffer, "image");
		else
			std::cout << "Buffer for gaussian blur was never set\n";

		parentBuffer->add(*tempTexture);
		parentBuffer->fill([this]() {
			bindToScreen();
		}, clearColor);

		//2. Draw final image via vertical blurring of the previous (horizontally) blurred image
		shader.bind<int>(horLocation, false);
		addTextureSampler(*tempTexture, "image");

	}

	std::vector<float> GaussianBlurBase::computeKernel(float sigma) const {
		std::vector<float> kernel = std::vector<float>(kernelSize);

		float s = 2.f * sigma * sigma;
		float sum = 0.f;
		for (int x = 0; x < kernelSize; x++) {
			kernel[x] = (exp(-(x * x) / s)) / (PI * s);
			sum += (x == 0) ? kernel[x] : 2.f * kernel[x];
		}

		for (int x = 0; x < kernelSize; x++)
			kernel[x] /= sum;

		return kernel;
	}

	float GaussianBlurBase::getSigma() const {
		return sigma;
	}

	void GaussianBlurBase::setSigma(float value) {
		if (sigma != value && value > 0.f) {
			sigma = value;
			updateKernel();
			
		}
	}

	void GaussianBlurBase::setKernelsize(unsigned int size) {
		kernelSize = (size % 2 == 0) ? size + 1 : size;
		updateKernel();
	}


	void GaussianBlurBase::updateKernel() {
		linearKernel.convert(computeKernel(sigma));
		linearKernel.bind(shader);
	}


	GaussianBlur::GaussianBlur(KernelSize kernelSize, float sigma)
		: GaussianBlurBase("renderer/postprocessing/gaussianblur" + std::to_string((int)kernelSize) + ".frag", sigma) {
		
		int size;
		switch (kernelSize) {
			case KernelSize::Medium:
				size = 9;
				break;
			case KernelSize::Large:
			case KernelSize::Depth:
				size = 17;
				break;
			case KernelSize::Huge:
				size = 27;
				break;
			default:
				size = 5;
				break;
		}

		setKernelsize(size);
	}




	SeparatedGaussian::SeparatedGaussian(float sigma) 
		: GaussianBlurBase("renderer/postprocessing/gaussianseparated.frag", sigma) {

		setKernelsize(7);
		setSigma(sigma);
	}

	float SeparatedGaussian::getSigmaR() const {
		return sigmaR;
	}

	float SeparatedGaussian::getSigmaG() const {
		return sigmaG;
	}

	float SeparatedGaussian::getSigmaB() const {
		return sigmaB;
	}

	void SeparatedGaussian::setSigma(float value) {
		GaussianBlurBase::setSigma(value);

		setSigmaR(value);
		setSigmaG(value);
		setSigmaB(value);
	}

	void SeparatedGaussian::setSigmaR(float value) {
		if (sigmaR != value && value > 0.f) {
			sigmaR = value;
			
			kernelR.convert(computeKernel(sigmaR));
			kernelR.bind(shader, "weightsR", "offsetsR");

		}
	}

	void SeparatedGaussian::setSigmaG(float value) {
		if (sigmaG != value && value > 0.f) {
			sigmaG = value;

			kernelG.convert(computeKernel(sigmaG));
			kernelG.bind(shader, "weightsG", "offsetsG");

		}
	}

	void SeparatedGaussian::setSigmaB(float value) {
		if (sigmaB != value && value > 0.f) {
			sigmaB = value;

			kernelB.convert(computeKernel(sigmaB));
			kernelB.bind(shader, "weightsB", "offsetsB");

		}
	}


	BilateralFilter::BilateralFilter(float sigma, float factor)
		: GaussianBlurBase("renderer/postprocessing/bilateral.frag", sigma), sigma2(factor) {
	
		setKernelsize(7);
	}

	BilateralFilter::BilateralFilter(string shaderPath, float sigma, float factor)
		: GaussianBlurBase(shaderPath, sigma), sigma2(factor) {}


	void BilateralFilter::init(const PostProcessingParameter& parameter) {
		GaussianBlurBase::init(parameter);

		shader.bind<float>("sigma", sigma2);
	}

	float BilateralFilter::getSigma() const {
		return sigma2;
	}

	void BilateralFilter::setSigma(float value) {
		if (sigma2 != value && value >= 0.f && value <= 1.f) {
			sigma2 = value;

			shader.bind<float>("sigma", sigma2);
		}
	}


	BilateralDepthFilter::BilateralDepthFilter(float sigma, float factor)
		: BilateralFilter("renderer/postprocessing/bilateraldepth.frag", sigma, factor) {}

	void BilateralDepthFilter::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addTextureSampler(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
	}



	SobelBlur::SobelBlur(SobelFilter& sobel, float sigma, bool depth)
		: GaussianBlurBase("renderer/postprocessing/sobelblur.frag", sigma), sobel(sobel), depth(depth) {
	
		setKernelsize(7);
	}

	SobelBlur::~SobelBlur() {
		if (sobelTexture != nullptr) delete sobelTexture;
	}


	void SobelBlur::setImage(const Texture& texture) {
		GaussianBlurBase::setImage(texture);

		//Use default image if sobel shouldn't use depth buffer ...
		if(!depth)
			sobel.setImage(texture);
		//... or if depth texture wasn't linked properly
		else {
			const Texture& buffer = getImage();
			if (buffer.isEmpty()) {
				sobel.setImage(texture);

				std::cout << "Use fallback texture for sobel blur "  
					<< "because depth texture hasn't been linked\n";
			}
		}
	}

	void SobelBlur::init(const PostProcessingParameter& parameter) {
		GaussianBlurBase::init(parameter);

		sobelTexture = new RenderTexture(resolution, ColorType::RGB16, WrapMode::ClampEdge, FilterMode::None);
		sobel.init(parameter);

		addTextureSampler(*sobelTexture, "sobel");
	}

	void SobelBlur::drawSubImages() {
		parentBuffer->add(*sobelTexture);
		parentBuffer->fill(sobel, clearColor);
	}

	void SobelBlur::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		sobel.setImage(*maps[WorldMaps::PositionRoughness]);
	}
	
	float SobelBlur::getScale() const {
		return sobel.getScale();
	}
	
	void SobelBlur::setScale(float value) {
		sobel.setScale(value);
	}


}
