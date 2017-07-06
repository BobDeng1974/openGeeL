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

	GaussianBlur::GaussianBlur(float sigma)
		: GaussianBlur("renderer/postprocessing/gaussianblur.frag", sigma) {}

	GaussianBlur::GaussianBlur(string shaderPath, float sigma)
		: PostProcessingEffect(shaderPath), mainBuffer(nullptr), sigma(sigma), amount(1) {

		updateKernel();
	}


	void GaussianBlur::setBuffer(const Texture& texture) {
		PostProcessingEffect::setBuffer(texture);

		mainBuffer = &texture;
	}

	void GaussianBlur::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		frameBuffers[0].init(buffer.getWidth(), buffer.getHeight());
		frameBuffers[1].init(buffer.getWidth(), buffer.getHeight());

		bindKernel();
		horLocation = shader.getLocation("horizontal");
	}

	std::vector<float> GaussianBlur::computeKernel(float sigma) const {
		std::vector<float> kernel = std::vector<float>(kernelSize);

		double s = 2.0 * sigma * sigma;
		double sum = 0.0;
		for (int x = 0; x < kernelSize; x++) {
			kernel[x] = (exp(-(x * x) / s)) / (PI * s);
			sum += (x == 0) ? kernel[x] : 2.0 * kernel[x];
		}

		for (int x = 0; x < kernelSize; x++)
			kernel[x] /= sum;

		return kernel;
	}

	float GaussianBlur::getSigma() const {
		return sigma;
	}

	void GaussianBlur::setSigma(float value) {
		if (sigma != value && value > 0.f) {
			sigma = value;
			updateKernel();
			
			shader.use();
			bindKernel();
		}
	}

	unsigned int GaussianBlur::getStrength() const {
		return amount;
	}

	void GaussianBlur::setStrength(unsigned int value) {
		amount = 2 * value - 1;
		if (amount < 1)
			amount = 1;
		else if (amount > 10)
			amount = 10;
	}

	void GaussianBlur::bindValues() {
		bool horizontal = true;
		bool first = true;
		
		for (unsigned int i = 0; i < amount; i++) {
			frameBuffers[horizontal].bind();
			shader.setInteger(horLocation, horizontal);

			//Pick committed color buffer the first time and then the previous blurred buffer
			if (first) {
				first = false;
				//Use the original the first time
				if (mainBuffer != nullptr)
					addBuffer(*mainBuffer, "image");
				else
					std::cout << "Buffer for gaussian blur was never set\n";
			}
			else {
				//Then use the previously blurred image
				addBuffer(frameBuffers[!horizontal].getTexture(), "image");
			}

			//Render Call
			glClear(GL_DEPTH_BUFFER_BIT);
			bindToScreen();
			horizontal = !horizontal;
		}
		
		//Set values for final blur call
		shader.setInteger(horLocation, horizontal);
		addBuffer(frameBuffers[!horizontal].getTexture(), "image");

		parentBuffer->bind();
	}

	void GaussianBlur::setKernelsize(unsigned int size) {
		kernelSize = (size % 2 == 0) ? size + 1 : size;
		updateKernel();
	}

	void GaussianBlur::bindKernel() const {
		linearKernel.bind(shader);
	}

	void GaussianBlur::updateKernel() {
		linearKernel.convert(computeKernel(sigma));
	}



	BilateralFilter::BilateralFilter(float sigma, float factor)
		: GaussianBlur("renderer/postprocessing/bilateral.frag", sigma), sigma2(factor) {}

	BilateralFilter::BilateralFilter(string shaderPath, float sigma, float factor)
		: GaussianBlur(shaderPath, sigma), sigma2(factor) {}


	void BilateralFilter::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		GaussianBlur::init(screen, buffer);

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
		addBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
	}



	SobelBlur::SobelBlur(SobelFilter& sobel, float sigma, bool depth)
		: GaussianBlur("renderer/postprocessing/sobelblur.frag", sigma), sobel(sobel), depth(depth) {}


	void SobelBlur::setBuffer(const Texture& texture) {
		GaussianBlur::setBuffer(texture);

		//Use default image if sobel shouldn't use depth buffer ...
		if(!depth)
			sobel.setBuffer(texture);
		//... or if depth texture wasn't linked properly
		else {
			const Texture& buffer = getBuffer();
			if (buffer.isEmpty()) {
				sobel.setBuffer(texture);

				std::cout << "Use fallback texture for sobel blur "  
					<< "because depth texture hasn't been linked\n";
			}
		}
	}

	void SobelBlur::init(ScreenQuad & screen, const FrameBuffer& buffer) {
		GaussianBlur::init(screen, buffer);

		sobelBuffer.init(buffer.getWidth(), buffer.getHeight());
		sobel.init(screen, sobelBuffer);

		addBuffer(sobelBuffer.getTexture(), "sobel");
	}

	void SobelBlur::bindValues() {
		sobelBuffer.fill(sobel);

		shader.use();
		GaussianBlur::bindValues();
	}

	void SobelBlur::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		sobel.setBuffer(*maps[WorldMaps::PositionRoughness]);
	}
	
	float SobelBlur::getScale() const {
		return sobel.getScale();
	}
	
	void SobelBlur::setScale(float value) {
		sobel.setScale(value);
	}

}
