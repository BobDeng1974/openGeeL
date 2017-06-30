#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/framebuffer.h"
#include "postprocessing.h"

using namespace std;

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string fragmentPath)
		: PostProcessingEffect("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffect::PostProcessingEffect(string vertexPath, string fragmentPath)
		: shader(RenderShader(vertexPath.c_str(), fragmentPath.c_str())), onlyEffect(false) {}

	unsigned int PostProcessingEffect::getBuffer() const {
		return shader.getMap("image");
	}

	void PostProcessingEffect::setBuffer(const ColorBuffer& buffer) {
		setBuffer(buffer.getTexture());
	}

	void PostProcessingEffect::setBuffer(const Texture& texture) {
		setBuffer(texture.getID());
	}

	void PostProcessingEffect::setBuffer(unsigned int buffer) {
		shader.addMap(buffer, "image");
	}

	void PostProcessingEffect::setScreen(ScreenQuad& screen) {
		this->screen = &screen;
	}

	void PostProcessingEffect::addBuffer(const Texture& texture, const std::string& name) {
		addBuffer(texture.getID(), name);
	}

	void PostProcessingEffect::addBuffer(unsigned int id, const std::string& name) {
		shader.addMap(id, name);
	}

	void PostProcessingEffect::init(ScreenQuad& screen, const FrameBuffer& buffer) {
		this->screen = &screen;

		setParent(buffer);
		shader.use();
	}

	void PostProcessingEffect::draw() {
		shader.use();

		bindValues();
		bindToScreen();
	}

	void PostProcessingEffect::bindToScreen() {
		shader.loadMaps();
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