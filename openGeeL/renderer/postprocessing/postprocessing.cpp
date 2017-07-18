#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/colorbuffer.h"
#include "postprocessing.h"

using namespace std;

namespace geeL {

	PostProcessingEffect::PostProcessingEffect(string fragmentPath)
		: PostProcessingEffect("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffect::PostProcessingEffect(string vertexPath, string fragmentPath)
		: shader(RenderShader(vertexPath.c_str(), fragmentPath.c_str())), onlyEffect(false) {}


	const Texture& PostProcessingEffect::getImageBuffer() const {
		return *shader.getMap("image");
	}

	void PostProcessingEffect::setImageBuffer(const ColorBuffer& buffer) {
		setImageBuffer(buffer.getTexture());
	}

	void PostProcessingEffect::setImageBuffer(const Texture& texture) {
		shader.addMap(texture, "image");
	}


	void PostProcessingEffect::addImageBuffer(const Texture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}


	void PostProcessingEffect::init(ScreenQuad& screen, IFrameBuffer& buffer, const Resolution& resolution) {
		this->screen = &screen;
		this->resolution = resolution;

		setParent(buffer);
		shader.use();
	}

	void PostProcessingEffect::draw() {
		shader.use();

		bindValues();
		bindToScreen();
	}

	void PostProcessingEffect::fill() {
		if (parentBuffer != nullptr)
			parentBuffer->fill(*this);
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