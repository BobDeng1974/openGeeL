#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
#include "gaussianblur.h"

namespace geeL {

	GaussianBlur::GaussianBlur(unsigned int strength)
		: PostProcessingEffect("renderer/postprocessing/gaussianblur.frag") {
		
		amount = 4 + 2 * strength;
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
		
		//Blur one time less because last blurring will be done when drawing to screen
		for (int i = 0; i < (amount - 1); i++) {

			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[horizontal].fbo);
			shader.setInteger("horizontal", horizontal);

			//Pick committed color buffer the first time and then the previous blurred buffer
			if (first) 
				first = false;
			else
				setBuffer(frameBuffers[!horizontal].color);

			//Render Call
			bindToScreen();

			horizontal = !horizontal;
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		//Set color buffer to the last blurred buffer
		setBuffer(frameBuffers[0].color);
	}

	void GaussianBlur::setKernel(float newKernel[5]) {
		for (int i = 0; i < 5; i++)
			kernel[i] = newKernel[i];
	}
}