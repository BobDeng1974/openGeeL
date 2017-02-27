#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../utility/screenquad.h"
#include "postprocessing.h"

using namespace std;

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string fragmentPath)
		: PostProcessingEffect("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffect::PostProcessingEffect(string vertexPath, string fragmentPath)
		: shader(Shader(vertexPath.c_str(), fragmentPath.c_str())), onlyEffect(false) {
	
		shader.mapOffset = 1;
		buffers.push_back(0);
	}

	unsigned int PostProcessingEffect::getBuffer() const {
		return buffers.front();
	}

	void PostProcessingEffect::setBuffer(unsigned int buffer) {
		buffers.front() = buffer;
	}

	void PostProcessingEffect::setBuffer(std::list<unsigned int> buffers) {
		this->buffers.clear();
		int counter = 0;
		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			this->buffers.push_back(*it);

			counter++;
			if (counter >= maxBuffers)
				return;
		}
	}

	void PostProcessingEffect::addBuffer(std::list<unsigned int> buffers) {
		int counter = this->buffers.size();
		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			this->buffers.push_back(*it);

			counter++;
			if (counter >= maxBuffers)
				return;
		}
	}

	void PostProcessingEffect::init(ScreenQuad& screen) {
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

	string PostProcessingEffect::toString() const {
		return "Post effect with shader: " + shader.name;
	}

	void PostProcessingEffect::effectOnly(bool only) {
		onlyEffect = only;
	}

	bool PostProcessingEffect::getEffectOnly() const {
		return onlyEffect;
	}
}