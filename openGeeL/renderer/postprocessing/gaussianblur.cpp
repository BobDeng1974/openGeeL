#define GLEW_STATIC
#include <glew.h>
#include "texturing/texture.h"
#include "texturing/rendertexture.h"
#include "texturing/textureprovider.h"
#include "shader/rendershader.h"
#include "shader/shaderreader.h"
#include "primitives/screenquad.h"
#include "framebuffer/framebuffer.h"
#include "sobel.h"
#include "gaussianblur.h"
#include <iostream>

const float PI = 3.141592653;

using namespace glm;
using namespace std;

namespace geeL {

	unsigned int sizeHelper(unsigned int kernelSize) {
		unsigned int s = (kernelSize % 2 == 0) ? kernelSize + 1 : kernelSize;
		return (s + 1) / 2;
	}


	GaussianBlur::GaussianBlur(float sigma, unsigned int kernelSize)
		: GaussianBlur("shaders/postprocessing/gaussianblur.frag", sigma, kernelSize) {}

	GaussianBlur::GaussianBlur(string shaderPath, float sigma, unsigned int kernelSize)
		: PostProcessingEffectFS(defaultVertexPath, shaderPath,
			StringReplacement("^const int kernelSize =\\s+([0-9]+){1};\\s?", 
				std::to_string(sizeHelper(kernelSize)), 1))
		, mainBuffer(nullptr)
		, sigma(sigma) {

		setKernelsize(kernelSize);
	}

	GaussianBlur::~GaussianBlur() {
		if (tempTexture != nullptr) delete tempTexture;
	}


	void GaussianBlur::setImage(const ITexture& texture) {
		PostProcessingEffectFS::setImage(texture);

		mainBuffer = &texture;
	}

	void GaussianBlur::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		ResolutionPreset preset = provider->getClosestPreset(resolution);
		tempTexture = &provider->requestTextureManual(preset, ColorType::RGB16, 
			FilterMode::Linear, WrapMode::ClampEdge);

		horLocation = shader.getLocation("horizontal");
	}

	void GaussianBlur::drawSubImages() {
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

	std::vector<float> GaussianBlur::computeKernel(float sigma, unsigned int kernelSize, bool halvedDistribution) {
		std::vector<float> kernel = std::vector<float>(kernelSize);

		float h = halvedDistribution ? 1.f : 2.f;
		float s = 2.f * sigma * sigma;
		float sum = 0.f;
		for (int x = 0; x < int(kernelSize); x++) {
			kernel[x] = (exp(-(x * x) / s)) / (PI * s);
			sum += (x == 0) ? kernel[x] : h * kernel[x];
		}

		for (unsigned int x = 0; x < kernelSize; x++)
			kernel[x] /= sum;

		return kernel;
	}

	std::vector<float> GaussianBlur::computeKernel(float sigma) const {
		return std::move(computeKernel(sigma, kernelSize));
	}

	float GaussianBlur::getSigma() const {
		return sigma;
	}

	void GaussianBlur::setSigma(float value) {
		if (sigma != value && value > 0.f) {
			sigma = value;
			updateKernel();
			
		}
	}

	void GaussianBlur::setKernelsize(unsigned int size) {
		kernelSize = (size % 2 == 0) ? size + 1 : size;
		updateKernel();
	}


	void GaussianBlur::updateKernel() {
		linearKernel.convert(computeKernel(sigma));
		linearKernel.bind(shader);
	}





	SeparatedGaussian::SeparatedGaussian(float sigma, unsigned int kernelSize, float falloff)
		: SeparatedGaussian("shaders/postprocessing/gaussianseparated.frag", sigma, kernelSize, falloff) {}

	SeparatedGaussian::SeparatedGaussian(const std::string& shaderPath, float sigma, unsigned int kernelSize, float falloff) 
		: GaussianBlur(shaderPath, sigma, kernelSize) {

		setSigma(sigma);
		setFalloff(falloff);
	}

	void SeparatedGaussian::init(const PostProcessingParameter& parameter) {
		GaussianBlur::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "position");
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

	float SeparatedGaussian::getFalloff() const {
		return falloff;
	}

	void SeparatedGaussian::setSigma(float value) {
		GaussianBlur::setSigma(value);

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

	void SeparatedGaussian::setFalloff(float value) {
		if (falloff != value && value > 0.f) {
			falloff = value;

			shader.bind<float>("falloff", value);
		}
	}


	BilateralFilter::BilateralFilter(float sigma, unsigned int kernelSize, float factor)
		: GaussianBlur("shaders/postprocessing/bilateral.frag", sigma, kernelSize)
		, sigma2(factor) {}

	BilateralFilter::BilateralFilter(string shaderPath, float sigma, unsigned int kernelSize, float factor)
		: GaussianBlur(shaderPath, sigma, kernelSize), sigma2(factor) {}


	void BilateralFilter::init(const PostProcessingParameter& parameter) {
		GaussianBlur::init(parameter);

		assert(provider != nullptr);
		addTextureSampler(provider->requestPosition(), "gPositionDepth");

		shader.bind<float>("sigma", sigma2);
	}

	float BilateralFilter::getFactor() const {
		return sigma2;
	}

	void BilateralFilter::setFactor(float value) {
		if (sigma2 != value && value >= 0.f && value <= 1.f) {
			sigma2 = value;

			shader.bind<float>("sigma", sigma2);
		}
	}


	BilateralDepthFilter::BilateralDepthFilter(float sigma, unsigned int kernelSize, float factor)
		: BilateralFilter("shaders/postprocessing/bilateraldepth.frag", 
			sigma, kernelSize, factor) {}



	SobelBlur::SobelBlur(SobelFilter& sobel, float sigma, unsigned int kernelSize, bool depth)
		: GaussianBlur("shaders/postprocessing/sobelblur.frag", sigma, kernelSize)
		, sobel(sobel)
		, depth(depth) {}

	SobelBlur::~SobelBlur() {
		if (sobelTexture != nullptr) delete sobelTexture;
	}


	void SobelBlur::setImage(const ITexture& texture) {
		GaussianBlur::setImage(texture);

		//Use default image if sobel shouldn't use depth buffer ...
		if(!depth)
			sobel.setImage(texture);
		//... or if depth texture wasn't linked properly
		else {
			const ITexture& buffer = getImage();
			if (buffer.isEmpty()) {
				sobel.setImage(texture);

				std::cout << "Use fallback texture for sobel blur "  
					<< "because depth texture hasn't been linked\n";
			}
		}
	}

	void SobelBlur::init(const PostProcessingParameter& parameter) {
		GaussianBlur::init(parameter);

		assert(provider != nullptr);
		sobel.setImage(provider->requestPosition());

		ResolutionScale scale(resolution.getScale());
		ResolutionPreset preset = getRenderPreset(scale);

		sobelTexture = &provider->requestTextureManual(preset, ColorType::RGB16,
			FilterMode::None, WrapMode::ClampEdge);
		sobel.init(parameter);

		addTextureSampler(*sobelTexture, "sobel");
	}

	void SobelBlur::drawSubImages() {
		parentBuffer->add(*sobelTexture);
		parentBuffer->fill(sobel, clearColor);
	}

	
	float SobelBlur::getScale() const {
		return sobel.getScale();
	}
	
	void SobelBlur::setScale(float value) {
		sobel.setScale(value);
	}


}
