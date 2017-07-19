#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/colorbuffer.h"
#include "postprocessing.h"

using namespace std;

namespace geeL {

	PostProcessingEffectFS::PostProcessingEffectFS(string fragmentPath)
		: PostProcessingEffectFS("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffectFS::PostProcessingEffectFS(string vertexPath, string fragmentPath)
		: shader(RenderShader(vertexPath.c_str(), fragmentPath.c_str())), onlyEffect(false) {}


	const Texture& PostProcessingEffectFS::getImageBuffer() const {
		return *shader.getMap("image");
	}

	void PostProcessingEffectFS::setImageBuffer(const ColorBuffer& buffer) {
		setImageBuffer(buffer.getTexture(0));
	}

	void PostProcessingEffectFS::setImageBuffer(const Texture& texture) {
		shader.addMap(texture, "image");
	}


	void PostProcessingEffectFS::addImageBuffer(const Texture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}


	void PostProcessingEffectFS::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		this->screen = &screen;
		this->resolution = resolution;

		setParent(buffer);
		shader.use();
	}

	void PostProcessingEffectFS::draw() {
		shader.use();

		bindValues();
		bindToScreen();
	}

	void PostProcessingEffectFS::fill() {
		if (parentBuffer != nullptr)
			parentBuffer->fill(*this);
	}

	void PostProcessingEffectFS::bindToScreen() {
		shader.loadMaps();
		screen->draw();
	}


	string PostProcessingEffectFS::toString() const {
		return "Post effect with shader: " + shader.name;
	}

	void PostProcessingEffectFS::effectOnly(bool only) {
		onlyEffect = only;
	}

	bool PostProcessingEffectFS::getEffectOnly() const {
		return onlyEffect;
	}

	const Resolution& PostProcessingEffectFS::getResolution() const {
		return resolution;
	}

	void PostProcessingEffectFS::setResolution(const Resolution& value) {
		resolution = value;
	}
}