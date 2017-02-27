#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
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

		amount = 1 + 2 * strength;
		if (amount > maxAmount)
			amount = maxAmount;
	}

	void GaussianBlur::init(ScreenQuad& screen) {
		PostProcessingEffect::init(screen);

		frameBuffers[0].init(screen.width, screen.height);
		frameBuffers[1].init(screen.width, screen.height);
	}

	void GaussianBlur::bindValues() {
		PostProcessingEffect::bindValues();

		bool horizontal = true;
		bool first = true;

		//Set kernel
		for (int i = 0; i < 5; i++) {
			string name = "kernel[" + std::to_string(i) + "]";
			shader.setFloat(name, kernel[i]);
		}
		
		for (int i = 0; i < amount; i++) {
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
				
		FrameBuffer::bind(parentFBO);
	}

	void GaussianBlur::bindToScreen() {
		shader.use();

		std::list<unsigned int> buffs = { currBuffer };
		shader.loadMaps(buffs);
		screen->draw();
	}

	void GaussianBlur::setKernel(float newKernel[5]) {
		for (int i = 0; i < 5; i++)
			kernel[i] = newKernel[i];
	}



	BilateralFilter::BilateralFilter(unsigned int strength, float sigma)
		: GaussianBlur(strength, "renderer/postprocessing/bilateral.frag"), sigma(sigma) {}

	BilateralFilter::BilateralFilter(string shaderPath, unsigned int strength, float sigma)
		: GaussianBlur(strength, shaderPath), sigma(sigma) {}


	void BilateralFilter::bindValues() {
		shader.setFloat("sigma", sigma);

		GaussianBlur::bindValues();
	}



	BilateralDepthFilter::BilateralDepthFilter(unsigned int strength, float sigma)
		: BilateralFilter("renderer/postprocessing/bilateraldepth.frag", strength, sigma) {}


	void BilateralDepthFilter::bindValues() {
		BilateralFilter::bindValues();

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

	void SobelBlur::init(ScreenQuad & screen) {
		GaussianBlur::init(screen);

		sobelBuffer.init(screen.width, screen.height);
		sobel.init(screen);
		sobel.setParentFBO(sobelBuffer.fbo);

		buffers.push_back(sobelBuffer.getColorID());
	}

	void SobelBlur::bindValues() {
		sobelBuffer.fill(sobel);

		shader.use();
		shader.setInteger("sobel", shader.mapOffset + 1);
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
