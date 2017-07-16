#define GLEW_STATIC
#include <glew.h>
#include "../texturing/texture.h"
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
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
		: PostProcessingEffect(shaderPath), mainBuffer(nullptr), sigma(sigma) {

		updateKernel();
	}


	void GaussianBlurBase::setImageBuffer(const Texture& texture) {
		PostProcessingEffect::setImageBuffer(texture);

		mainBuffer = &texture;
	}

	void GaussianBlurBase::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		tempBuffer.init(buffer.getResolution(), ColorType::RGB16, FilterMode::Linear, WrapMode::ClampEdge);

		bindKernel();
		horLocation = shader.getLocation("horizontal");
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
			
			shader.use();
			bindKernel();
		}
	}

	void GaussianBlurBase::bindValues() {
		
		//1. Fill the temporary buffer with the result of the horizontal blurring
		shader.setInteger(horLocation, true);

		if (mainBuffer != nullptr)
			addImageBuffer(*mainBuffer, "image");
		else
			std::cout << "Buffer for gaussian blur was never set\n";
			
		tempBuffer.bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		bindToScreen();


		//2. Draw final image via vertical blurring of the previous (horizontally) blurred image
		shader.setInteger(horLocation, false);
		addImageBuffer(tempBuffer.getTexture(), "image");

		parentBuffer->bind();
	}

	void GaussianBlurBase::setKernelsize(unsigned int size) {
		kernelSize = (size % 2 == 0) ? size + 1 : size;
		updateKernel();
	}

	void GaussianBlurBase::bindKernel() const {
		linearKernel.bind(shader);
	}

	void GaussianBlurBase::updateKernel() {
		linearKernel.convert(computeKernel(sigma));
	}


	GaussianBlur::GaussianBlur(KernelSize kernelSize, float sigma)
		: GaussianBlurBase("renderer/postprocessing/gaussianblur" + std::to_string((int)kernelSize) + ".frag", sigma) {
		
		int size;
		switch (kernelSize) {
			case KernelSize::Medium:
				size = 9;
				break;
			case KernelSize::Large:
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


	BilateralFilter::BilateralFilter(float sigma, float factor)
		: GaussianBlurBase("renderer/postprocessing/bilateral.frag", sigma), sigma2(factor) {}

	BilateralFilter::BilateralFilter(string shaderPath, float sigma, float factor)
		: GaussianBlurBase(shaderPath, sigma), sigma2(factor) {}


	void BilateralFilter::init(ScreenQuad& screen, IFrameBuffer& buffer) {
		GaussianBlurBase::init(screen, buffer);

		shader.setFloat("sigma", sigma2);
	}

	float BilateralFilter::getSigma() const {
		return sigma2;
	}

	void BilateralFilter::setSigma(float value) {
		if (sigma2 != value && value >= 0.f && value <= 1.f) {
			sigma2 = value;

			shader.use();
			shader.setFloat("sigma", sigma2);
		}
	}


	BilateralDepthFilter::BilateralDepthFilter(float sigma, float factor)
		: BilateralFilter("renderer/postprocessing/bilateraldepth.frag", sigma, factor) {}

	void BilateralDepthFilter::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addImageBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
	}



	SobelBlur::SobelBlur(SobelFilter& sobel, float sigma, bool depth)
		: GaussianBlurBase("renderer/postprocessing/sobelblur.frag", sigma), sobel(sobel), depth(depth) {
	
		setKernelsize(7);
	}


	void SobelBlur::setImageBuffer(const Texture& texture) {
		GaussianBlurBase::setImageBuffer(texture);

		//Use default image if sobel shouldn't use depth buffer ...
		if(!depth)
			sobel.setImageBuffer(texture);
		//... or if depth texture wasn't linked properly
		else {
			const Texture& buffer = getImageBuffer();
			if (buffer.isEmpty()) {
				sobel.setImageBuffer(texture);

				std::cout << "Use fallback texture for sobel blur "  
					<< "because depth texture hasn't been linked\n";
			}
		}
	}

	void SobelBlur::init(ScreenQuad & screen, IFrameBuffer& buffer) {
		GaussianBlurBase::init(screen, buffer);

		sobelBuffer.init(buffer.getResolution());
		sobel.init(screen, sobelBuffer);

		addImageBuffer(sobelBuffer.getTexture(), "sobel");
	}

	void SobelBlur::bindValues() {
		sobelBuffer.fill(sobel);

		shader.use();
		GaussianBlurBase::bindValues();
	}

	void SobelBlur::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		sobel.setImageBuffer(*maps[WorldMaps::PositionRoughness]);
	}
	
	float SobelBlur::getScale() const {
		return sobel.getScale();
	}
	
	void SobelBlur::setScale(float value) {
		sobel.setScale(value);
	}

	
}
