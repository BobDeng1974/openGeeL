#define GLEW_STATIC
#include <glew.h>
#include "../shader/shader.h"
#include "../primitives/screenquad.h"
#include "../utility/framebuffer.h"
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

	void PostProcessingEffect::setBuffer(const FrameBuffer& buffer) {
		setBuffer(buffer.getColorID());
	}

	void PostProcessingEffect::setBuffer(unsigned int buffer) {
		buffers.front() = buffer;
	}

	void PostProcessingEffect::setBuffer(std::list<unsigned int> buffers) {
		this->buffers.clear();
		unsigned int counter = 0;
		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			this->buffers.push_back(*it);

			counter++;
			if (counter >= maxBuffers)
				return;
		}
	}

	void PostProcessingEffect::addBuffer(std::list<unsigned int> buffers) {
		size_t counter = this->buffers.size();
		for (auto it = buffers.begin(); it != buffers.end(); it++) {
			this->buffers.push_back(*it);

			counter++;
			if (counter >= maxBuffers)
				return;
		}
	}

	void PostProcessingEffect::init(ScreenQuad& screen, const FrameBufferInformation& info) {
		this->screen = &screen;

		setParentFBO(info.fbo);

		shader.use();
		shader.setInteger("image", shader.mapOffset);
	}

	void PostProcessingEffect::draw() {
		shader.use();

		bindValues();
		bindToScreen();
	}

	void PostProcessingEffect::bindToScreen() {
		shader.loadMaps(buffers);
		screen->draw();
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