#define GLEW_STATIC
#include <glew.h>
#include "../texturing/texture.h"
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "sobel.h"
#include "gaussianblur.h"
#include <iostream>

using namespace glm;
using namespace std;

namespace geeL {

	GaussianBlur::GaussianBlur(unsigned int strength)
		: GaussianBlur(strength, "renderer/postprocessing/gaussianblur.frag") {}

	GaussianBlur::GaussianBlur(unsigned int strength, string shaderPath)
		: PostProcessingEffect(shaderPath), mainBuffer(nullptr) {

		amount = 2 * strength - 1;
		if (amount < 1)
			amount = 1;
		else if (amount > maxAmount)
			amount = maxAmount;
	}

	void GaussianBlur::setBuffer(const Texture& texture) {
		PostProcessingEffect::setBuffer(texture);

		mainBuffer = &texture;
	}

	void GaussianBlur::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		PostProcessingEffect::init(screen, buffer);

		frameBuffers[0].init(buffer.getWidth(), buffer.getHeight());
		frameBuffers[1].init(buffer.getWidth(), buffer.getHeight());

		//Set kernel
		for (int i = 0; i < 5; i++) {
			string name = "kernel[" + std::to_string(i) + "]";
			shader.setFloat(name, kernel[i]);
		}

		horLocation = shader.getLocation("horizontal");
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

	void GaussianBlur::setKernel(float newKernel[5]) {
		shader.use();
		for (int i = 0; i < 5; i++) {
			kernel[i] = newKernel[i];

			string name = "kernel[" + std::to_string(i) + "]";
			shader.setFloat(name, kernel[i]);
		}
	}



	BilateralFilter::BilateralFilter(unsigned int strength, float sigma)
		: GaussianBlur(strength, "renderer/postprocessing/bilateral.frag"), sigma(sigma) {}

	BilateralFilter::BilateralFilter(string shaderPath, unsigned int strength, float sigma)
		: GaussianBlur(strength, shaderPath), sigma(sigma) {}


	void BilateralFilter::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		GaussianBlur::init(screen, buffer);

		shader.setFloat("sigma", sigma);
	}

	float BilateralFilter::getSigma() const {
		return sigma;
	}

	void BilateralFilter::setSigma(float value) {
		if (sigma != value && value >= 0.f && value <= 1.f) {
			sigma = value;

			shader.use();
			shader.setFloat("sigma", sigma);
		}
	}


	BilateralDepthFilter::BilateralDepthFilter(unsigned int strength, float sigma)
		: BilateralFilter("renderer/postprocessing/bilateraldepth.frag", strength, sigma) {}

	void BilateralDepthFilter::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		addBuffer(*maps[WorldMaps::PositionRoughness], "gPositionDepth");
	}



	SobelBlur::SobelBlur(SobelFilter& sobel, unsigned int strength)
		: GaussianBlur(strength, "renderer/postprocessing/sobelblur.frag"), sobel(sobel) {}


	void SobelBlur::setBuffer(const Texture& texture) {
		GaussianBlur::setBuffer(texture);

		sobel.setBuffer(texture);
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

	void  SobelBlur::addWorldInformation(map<WorldMaps, const Texture*> maps) {
		sobel.setBuffer(*maps[WorldMaps::PositionRoughness]);
	}
	
	float SobelBlur::getScale() const {
		return sobel.getScale();
	}
	
	void SobelBlur::setScale(float value) {
		sobel.setScale(value);
	}

}
