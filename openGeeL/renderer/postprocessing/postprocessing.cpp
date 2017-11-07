#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "shader/rendershader.h"
#include "primitives/screenquad.h"
#include "framebuffer/colorbuffer.h"
#include "utility/glguards.h"
#include "utility/defaults.h"
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

	bool PostProcessingEffect::isActive() const {
		return active;
	}

	void PostProcessingEffect::setActive(bool value) {
		active = value;
	}

	

	PostProcessingEffectFS::PostProcessingEffectFS(const string& fragmentPath)
		: PostProcessingEffectFS("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffectFS::PostProcessingEffectFS(const string& vertexPath, const string& fragmentPath)
		: shader(RenderShader(vertexPath.c_str(), fragmentPath.c_str())) {}


	const Texture& PostProcessingEffectFS::getImage() const {
		return *shader.getMap("image");
	}

	void PostProcessingEffectFS::setImage(const Texture& texture) {
		shader.addMap(texture, "image");
	}


	void PostProcessingEffectFS::addTextureSampler(const Texture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}

	void PostProcessingEffectFS::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}


	void PostProcessingEffectFS::init(const PostProcessingParameter& parameter) {
		this->screen = &parameter.screen;
		this->resolution = parameter.resolution;
		this->fallbackEffect = parameter.fallbackEffect;

		setParent(parameter.buffer);
	}

	void PostProcessingEffectFS::draw() {
		if (active) {
			bindValues();
			bindToScreen();

		}
		else if(fallbackEffect != nullptr) {
			fallbackEffect->init(PostProcessingParameter(ScreenQuad::get(), *parentBuffer, resolution));
			fallbackEffect->setImage(getImage());
			fallbackEffect->draw();
		}
		
	}

	void PostProcessingEffectFS::fill() {
		if (parentBuffer != nullptr)
			parentBuffer->fill(*this, clearColor);
	}

	void PostProcessingEffectFS::bindToScreen() {
		bool masked = mask != RenderMask::None;
		StencilGuard stencil(masked);

		if(masked) Masking::readMask(mask);

		shader.bindParameters();
		shader.loadMaps();
		screen->draw();
	}

	Shader& PostProcessingEffectFS::getShader() {
		return shader;
	}

	string PostProcessingEffectFS::toString() const {
		return "Post effect with shader: " + shader.name;
	}




	PostProcessingEffectCS::PostProcessingEffectCS(const std::string& path, Resolution groupSize) 
		: shader(path.c_str()), groupSize(groupSize) {}


	const Texture& PostProcessingEffectCS::getImage() const {
		return *shader.getMap("image");
	}


	void PostProcessingEffectCS::setImage(const Texture& texture) {
		shader.addMap(texture, "image");
	}

	void PostProcessingEffectCS::addTextureSampler(const Texture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}

	void PostProcessingEffectCS::addImageTexture(const Texture& texture, unsigned int bindingPosition) {
		shader.addImage(texture, bindingPosition);
	}

	void PostProcessingEffectCS::init(const PostProcessingParameter& parameter) {
		this->resolution = parameter.resolution;

		this->buffer = &parameter.buffer;
	}

	void PostProcessingEffectCS::draw() {
		bindValues();
		shader.bindParameters();
		
		//Read target texture from parent buffer and bind it
		const RenderTexture* target = buffer->getTexture();
		target->bindImage(0, AccessType::Write);
		shader.bind<glm::vec2>("resolution", target->getResolution());

		//Bind source textures from shader
		shader.loadMaps();
		shader.loadImages();

		unsigned int width  = resolution.getWidth() / groupSize.getWidth();
		unsigned int height = resolution.getHeight() / groupSize.getHeight();
		
		shader.invoke(width, height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
		
		buffer->pop();
	}

	void PostProcessingEffectCS::fill() {
		draw();
	}

	Shader& PostProcessingEffectCS::getShader() {
		return shader;
	}


	std::string PostProcessingEffectCS::toString() const {
		return "Post effect with shader: " + shader.name;
	}

}