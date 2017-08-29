#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "shader/rendershader.h"
#include "primitives/screenquad.h"
#include "framebuffer/colorbuffer.h"
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

	

	PostProcessingEffectFS::PostProcessingEffectFS(const string& fragmentPath)
		: PostProcessingEffectFS("renderer/shaders/screen.vert", fragmentPath) {}

	PostProcessingEffectFS::PostProcessingEffectFS(const string& vertexPath, const string& fragmentPath)
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
	}

	void PostProcessingEffectFS::draw() {
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

	Shader& PostProcessingEffectFS::getShader() {
		return shader;
	}




	string PostProcessingEffectFS::toString() const {
		return "Post effect with shader: " + shader.name;
	}



	PostProcessingEffectCS::PostProcessingEffectCS(const std::string& path, Resolution groupSize) 
		: shader(path.c_str()), groupSize(groupSize) {}


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
	}

	void PostProcessingEffectCS::draw() {
		bindValues();
		shader.bindParameters();
		
		//Read target texture from parent buffer and bind it
		const RenderTexture* target = buffer->getTexture();
		target->bindImage(0, AccessType::Write);

		Resolution r = target->getResolution();
		shader.bind<glm::vec2>("resolution", glm::vec2(r.getWidth(), r.getHeight()));

		//Bind source textures from shader
		shader.loadMaps(1);

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