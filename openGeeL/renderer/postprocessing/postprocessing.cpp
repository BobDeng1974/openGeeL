#define GLEW_STATIC
#include <glew.h>
#include <iostream>
#include "shader/rendershader.h"
#include "primitives/screenquad.h"
#include "framebuffer/colorbuffer.h"
#include "texturing/textureprovider.h"
#include "texturing/rendertexture.h"
#include "glwrapper/glguards.h"
#include "utility/defaults.h"
#include "postprocessing.h"

using namespace std;

namespace geeL {


	void PostProcessingEffect::init(const PostProcessingParameter& parameter) {
		this->resolution = parameter.resolution;
		this->provider = parameter.provider;

		setParent(parameter.buffer);
	}


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
		: PostProcessingEffectFS(defaultVertexPath, fragmentPath) {}

	PostProcessingEffectFS::PostProcessingEffectFS(const string& vertexPath, const string& fragmentPath)
		: shader(RenderShader(vertexPath.c_str(), fragmentPath.c_str(), nullptr, nullptr)) {}


	const ITexture& PostProcessingEffectFS::getImage() const {
		return *shader.getMap("image");
	}

	void PostProcessingEffectFS::setImage(const ITexture& texture) {
		shader.addMap(texture, "image");
	}


	void PostProcessingEffectFS::addTextureSampler(const ITexture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}

	void PostProcessingEffectFS::setRenderMask(RenderMask mask) {
		this->mask = mask;
	}


	void PostProcessingEffectFS::init(const PostProcessingParameter& parameter) {
		PostProcessingEffect::init(parameter);

		this->screen = &parameter.screen;
		this->fallbackEffect = parameter.fallbackEffect;
	}

	void PostProcessingEffectFS::draw() {
		if (active) {
			drawSubImages();
			bindToScreen();
		}
		else if(fallbackEffect != nullptr) {
			fallbackEffect->init(PostProcessingParameter(ScreenQuad::get(), *parentBuffer, resolution));
			fallbackEffect->setImage(getImage());
			fallbackEffect->draw();
		}
		
	}

	void PostProcessingEffectFS::fill() {
		if (parentBuffer != nullptr) {
			RenderTexture& source = provider->requestCurrentImage();
			RenderTexture& target = provider->requestDefaultTexture();

			setImage(source);

			parentBuffer->add(target);
			parentBuffer->fill(*this, clearColor);

			provider->updateCurrentImage(target);
		}
			
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
		: shader(path.c_str())
		, groupSize(groupSize)
		, target(nullptr) {}


	void PostProcessingEffectCS::setTextureTarget(const RenderTexture& target) {
		this->target = &target;
	}

	const ITexture& PostProcessingEffectCS::getImage() const {
		return *shader.getMap("image");
	}


	void PostProcessingEffectCS::setImage(const ITexture& texture) {
		shader.addMap(texture, "image");
	}

	void PostProcessingEffectCS::addTextureSampler(const ITexture& texture, const std::string& name) {
		shader.addMap(texture, name);
	}

	void PostProcessingEffectCS::addImageTexture(const ITexture& texture, unsigned int bindingPosition) {
		shader.addImage(texture, bindingPosition);
	}

	void PostProcessingEffectCS::init(const PostProcessingParameter& parameter) {
		PostProcessingEffect::init(parameter);
	}

	void PostProcessingEffectCS::draw() {
		fill();
	}

	void PostProcessingEffectCS::fill() {
		shader.bindParameters();

		bindTextureTargets();

		//Bind source textures from shader
		shader.loadMaps();
		shader.loadImages();

		unsigned int gw = groupSize.getWidth();
		unsigned int gh = groupSize.getHeight();
		unsigned int width  = (resolution.getWidth()  + gw - 1) / gw;
		unsigned int height = (resolution.getHeight() + gh - 1) / gh;

		shader.invoke(width, height, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void PostProcessingEffectCS::bindTextureTargets() {
		const RenderTexture& t = (target != nullptr) ? *target : provider->requestCurrentImage();
		t.bindImage(0, AccessType::All);
		shader.bind<glm::vec2>("resolution", t.getResolution());
	}


	Shader& PostProcessingEffectCS::getShader() {
		return shader;
	}


	std::string PostProcessingEffectCS::toString() const {
		return "Post effect with shader: " + shader.name;
	}

	
}