#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "postprocessscreen.h"
#include "postprocessing.h"

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string shaderPath) 
		: shader(Shader("renderer/shaders/screen.vert", shaderPath.c_str())) {}


	void PostProcessingEffect::setBuffer(unsigned int buffer) {
		this->buffer = buffer;
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
		glBindVertexArray(screen->vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}