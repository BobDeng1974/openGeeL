#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "../shader/rendershader.h"
#include "../primitives/screenquad.h"
#include "../framebuffer/colorbuffer.h"
#include "postprocessing.h"

using namespace std;

namespace geeL {


	void PostProcessingEffect::effectOnly(bool only) {
		onlyEffect = only;
	}

	bool PostProcessingEffect::getEffectOnly() const {
		return onlyEffect;
	}

	const Resolution& PostProcessingEffect::getResolution() const {
		return resolution;
	}

	void PostProcessingEffect::setResolution(const Resolution& value) {
		resolution = value;
	}


	PostProcessingEffectFS::PostProcessingEffectFS(string fragmentPath)
		: PostProcessingEffectFS("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffectFS::PostProcessingEffectFS(string vertexPath, string fragmentPath)
		: shader(RenderShader(vertexPath.c_str(), fragmentPath.c_str())) {}


	const Texture& PostProcessingEffectFS::getImageBuffer() const {
		return *shader.getMap("image");
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
		shader.bindParameters();
		shader.loadMaps();
		screen->draw();
	}


	string PostProcessingEffectFS::toString() const {
		return "Post effect with shader: " + shader.name;
	}



	PostProcessingEffectCS::PostProcessingEffectCS(const std::string& path) : shader(path.c_str()) {}


	const Texture& PostProcessingEffectCS::getImageBuffer() const {
		return *shader.getMap("image");
	}


	void PostProcessingEffectCS::setImageBuffer(const Texture& texture) {
		shader.addMap(texture, "image");
	}

	void PostProcessingEffectCS::addImageBuffer(const Texture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}

	void PostProcessingEffectCS::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		this->resolution = resolution;

		this->buffer = &buffer;
		shader.use();
	}

	void PostProcessingEffectCS::draw() {
		shader.use();
		bindValues();

		//Read target texture from parent buffer and bind it
		const RenderTexture* target = buffer->getTexture();
		target->bindImage(0, AccessType::Write);

		//Bind source textures from shader
		shader.loadMaps(1);

		unsigned int width = resolution.getWidth() / 8;
		unsigned int height = (resolution.getHeight() + 7) / 8;
		
		shader.invoke(width, height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void PostProcessingEffectCS::fill() {
		fill();
	}


	std::string PostProcessingEffectCS::toString() const {
		return "Post effect with shader: " + shader.name;
	}


}