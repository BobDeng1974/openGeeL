#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "postprocessscreen.h"
#include "postprocessing.h"

#include <iostream>

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string fragmentPath)
		: shader(Shader("renderer/shaders/screen.vert", fragmentPath.c_str())) {}

	PostProcessingEffect::PostProcessingEffect(string vertexPath, string fragmentPath)
		: shader(Shader(vertexPath.c_str(), fragmentPath.c_str())) {}


	void PostProcessingEffect::setBuffer(unsigned int buffer) {
		buffers.clear();
		buffers.push_back(buffer);
	}

	void PostProcessingEffect::setBuffer(std::list<unsigned int> buffers) {
		this->buffers.clear();
		int counter = 0;
		for (std::list<unsigned int>::iterator it = buffers.begin(); it != buffers.end(); it++) {
			this->buffers.push_back(*it);

			counter++;
			if (counter >= maxBuffers)
				return;
		}
	}

	void PostProcessingEffect::setScreen(PostProcessingScreen& screen) {
		this->screen = &screen;
	}

	void PostProcessingEffect::draw() {
		shader.use();

		bindValues();
		bindToScreen();
	}

	void PostProcessingEffect::bindToScreen() {
		shader.use();
		glBindVertexArray(screen->vao);

		int counter = bindingStart;
		for (std::list<unsigned int>::iterator it = buffers.begin(); it != buffers.end(); it++) {
			glActiveTexture(GL_TEXTURE0 + counter);
			glBindTexture(GL_TEXTURE_2D, *it);

			counter++;
		}
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	void PostProcessingEffect::bindValues() {
		shader.setInteger("image", bindingStart);
	}
}