#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
#include "gaussianblur.h"

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

	WorldMaps BilateralDepthFilter::requiredWorldMaps() const {
		return (WorldMaps::RenderedImage | WorldMaps::PositionDepth);
	}

	WorldMatrices BilateralDepthFilter::requiredWorldMatrices() const {
		return WorldMatrices::None;
	}

	WorldVectors BilateralDepthFilter::requiredWorldVectors() const {
		return WorldVectors::None;
	}

	list<WorldMaps> BilateralDepthFilter::requiredWorldMapsList() const {
		return{ WorldMaps::RenderedImage, WorldMaps::PositionDepth };
	}

	void BilateralDepthFilter::addWorldInformation(list<unsigned int> maps,
		list<const mat4*> matrices, list<const vec3*> vectors) {

		if (maps.size() != 2)
			throw "Wrong number of texture maps attached to bilateral depth filter";

		setBuffer(maps);
	}
	
}