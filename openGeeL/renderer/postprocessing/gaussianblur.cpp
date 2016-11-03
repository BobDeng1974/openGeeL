#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
#include "gaussianblur.h"
#include <iostream>

namespace geeL {

	GaussianBlur::GaussianBlur(unsigned int strength)
		: PostProcessingEffect("renderer/postprocessing/gaussianblur.frag") {
		
		amount = 1 + 2 * strength;
		if (amount > maxAmount)
			amount = maxAmount;
	}

	void GaussianBlur::setScreen(ScreenQuad& screen) {
		PostProcessingEffect::setScreen(screen);

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
				currBuffer = frameBuffers[!horizontal].color;
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
}