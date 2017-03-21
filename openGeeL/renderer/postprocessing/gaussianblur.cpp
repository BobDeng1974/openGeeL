#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
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
		: PostProcessingEffect(shaderPath) {

		amount = 2 * strength - 1;
		if (amount < 1)
			amount = 1;
		else if (amount > maxAmount)
			amount = maxAmount;
	}

	void GaussianBlur::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		PostProcessingEffect::init(screen, info);

		frameBuffers[0].init(info.width, info.height);
		frameBuffers[1].init(info.width, info.height);

		//Set kernel
		for (int i = 0; i < 5; i++) {
			string name = "kernel[" + std::to_string(i) + "]";
			shader.setFloat(name, kernel[i]);
		}
	}

	void GaussianBlur::bindValues() {
		PostProcessingEffect::bindValues();

		bool horizontal = true;
		bool first = true;
		
		for (unsigned int i = 0; i < amount; i++) {
			frameBuffers[horizontal].bind();
			shader.setInteger("horizontal", horizontal);

			//Pick committed color buffer the first time and then the previous blurred buffer
			if (first) {
				first = false;
				//Use the original the first time
				currBuffer = buffers.front();
			}
			else {
				//Then use the previously blurred image
				currBuffer = frameBuffers[!horizontal].getColorID();
			}

			//Render Call
			glClear(GL_DEPTH_BUFFER_BIT);
			bindToScreen();
			horizontal = !horizontal;
		}
		
		//Set values for final blur call
		shader.setInteger("horizontal", horizontal);
		currBuffer = frameBuffers[!horizontal].getColorID();

		FrameBuffer::bind(parentFBO);
	}

	void GaussianBlur::bindToScreen() {
		shader.use();

		std::list<unsigned int> buffs = { currBuffer };
		shader.loadMaps(buffs);
		screen->draw();
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


	void BilateralFilter::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		GaussianBlur::init(screen, info);

		shader.setFloat("sigma", sigma);
	}


	BilateralDepthFilter::BilateralDepthFilter(unsigned int strength, float sigma)
		: BilateralFilter("renderer/postprocessing/bilateraldepth.frag", strength, sigma) {}


	void BilateralDepthFilter::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		BilateralFilter::init(screen, info);

		shader.setInteger("image", shader.mapOffset);
		shader.setInteger("gPositionDepth", shader.mapOffset + 1);
	}


	void BilateralDepthFilter::addWorldInformation(map<WorldMaps, unsigned int> maps,
		map<WorldMatrices, const glm::mat4*> matrices,
		map<WorldVectors, const glm::vec3*> vectors) {

		addBuffer( { maps[WorldMaps::PositionDepth] });
	}



	SobelBlur::SobelBlur(SobelFilter& sobel, unsigned int strength)
		: GaussianBlur(strength, "renderer/postprocessing/sobelblur.frag"), sobel(sobel) {}


	void SobelBlur::setBuffer(unsigned int buffer) {
		PostProcessingEffect::setBuffer(buffer);

		sobel.setBuffer(buffer);
	}

	void SobelBlur::init(ScreenQuad & screen, const FrameBufferInformation& info) {
		GaussianBlur::init(screen, info);

		shader.setInteger("sobel", shader.mapOffset + 1);

		sobelBuffer.init(info.width, info.height);
		sobel.init(screen, sobelBuffer.info);

		buffers.push_back(sobelBuffer.getColorID());
	}

	void SobelBlur::bindValues() {
		sobelBuffer.fill(sobel);

		shader.use();
		GaussianBlur::bindValues();
	}

	void SobelBlur::bindToScreen() {
		shader.use();

		std::list<unsigned int> buffs = { currBuffer, sobelBuffer.getColorID() };
		shader.loadMaps(buffs);
		screen->draw();
	}

	void  SobelBlur::addWorldInformation(map<WorldMaps, unsigned int> maps,
		map<WorldMatrices, const glm::mat4*> matrices,
		map<WorldVectors, const glm::vec3*> vectors) {

		sobel.setBuffer({ maps[WorldMaps::PositionDepth] });
	}
}
