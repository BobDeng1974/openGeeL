#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
#include "postprocessing.h"

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string fragmentPath)
		: PostProcessingEffect("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffect::PostProcessingEffect(string vertexPath, string fragmentPath)
		: shader(Shader(vertexPath.c_str(), fragmentPath.c_str())) {
	
		shader.mapOffset = 1;
		buffers.push_back(0);
	}


	void PostProcessingEffect::setBuffer(unsigned int buffer) {
		buffers.front() = buffer;
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

	void PostProcessingEffect::setScreen(ScreenQuad& screen) {
		this->screen = &screen;
	}

	void PostProcessingEffect::draw() {
		shader.use();

		bindValues();
		bindToScreen();
	}

	void PostProcessingEffect::bindToScreen() {
		shader.use();
		shader.loadMaps(buffers);
		screen->draw();
	}

	void PostProcessingEffect::bindValues() {
		shader.setInteger("image", shader.mapOffset);
	}
}