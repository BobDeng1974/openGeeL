#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <map>
#include "renderscene.h"
#include "window.h"
#include "inputmanager.h"
#include "shader/sceneshader.h"
#include "primitives/screenquad.h"
#include "texturing/texture.h"
#include "texturing/rendertexture.h"
#include "texturing/dynamictexture.h"
#include "texturing/imagetexture.h"
#include "framebuffer/gbuffer.h"
#include "framebuffer/tbuffer.h"
#include "postprocessing/drawdefault.h"
#include "postprocessing/ssao.h"
#include "postprocessing/postprocessing.h"
#include "cameras/camera.h"
#include "lights/lightmanager.h"
#include "guirenderer.h"
#include "lighting/deferredlighting.h"
#include "utility/viewport.h"
#include "application.h"
#include "deferredrenderer.h"

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow& window, Input& inputManager, SceneRender& lighting,
		RenderContext& context, DefaultPostProcess& def, GBuffer& gBuffer)
			: Renderer(window, inputManager, context), gBuffer(gBuffer),
				ssao(nullptr), lighting(lighting), toggle(0) {

		effects.push_back(PostEffectRender(nullptr, &def));
		init();
	}

	DeferredRenderer::~DeferredRenderer() {
		if(ssaoTexture != nullptr) delete ssaoTexture;

		delete texture1;
		delete texture2;
	}

	void DeferredRenderer::init() {
		auto func = [this](int key, int scancode, int action, int mode) 
			{ this->handleInput(key, scancode, action, mode); };

		input.addCallback(func);

		geometryPassFunc = [this]() { this->geometryPass(); };
		lightingPassFunc = [this]() { this->lightingPass(); this->forwardPass(); };

		texture1 = new RenderTexture(window->resolution, ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::None);
		texture2 = new RenderTexture(window->resolution, ColorType::RGBA16, WrapMode::ClampEdge, FilterMode::None);

		stackBuffer.initResolution(window->resolution);

		addRequester(lighting);
		ScreenQuad& defQuad = ScreenQuad::get();
		defQuad.init();
	}


	void DeferredRenderer::runStart() {
		window->makeCurrent();

		lighting.init(ScreenQuad::get(), stackBuffer, window->resolution);
		scene->updateProbes(); //Draw reflection probes once at beginning
		initDefaultEffect();

	}

	void DeferredRenderer::run() {
		//window->makeCurrent();
		glEnable(GL_DEPTH_TEST);

		Viewport::setForced(0, 0, window->resolution.getWidth(), window->resolution.getHeight());
		scene->lock();


		for (auto it(renderTextures.begin()); it != renderTextures.end(); it++) {
			DynamicRenderTexture& texture = **it;
			texture.update([this](const Camera& camera) { drawForward(camera); });
		}

		//Geometry pass
		gBuffer.fill(geometryPassFunc);

		//Hacky: Read camera depth from geometry pass and write it into the scene
		scene->forwardScreenInfo(gBuffer.screenInfo);

		//SSAO pass
		if (ssao != nullptr) {
			stackBuffer.push(*ssaoTexture);
			stackBuffer.fill(*ssao);
			FrameBuffer::resetSize(window->resolution);
		}

		scene->getLightmanager().update(*scene, &scene->getCamera());

		//Lighting & generic pass
		stackBuffer.push(*texture1);
		stackBuffer.fill(lightingPassFunc);

		//Forward pass
		bool copiedDepth = false;
		if (fBuffer != nullptr && scene->count(ShadingMethod::Forward) > 0) {
			fBuffer->fill([this]() {
				glClear(GL_DEPTH_BUFFER_BIT);
				fBuffer->copyDepth(gBuffer);
				scene->drawForward();
			});

			copiedDepth = true;
		}

		//Transparent pass
		if (fBuffer != nullptr && scene->count(ShadingMethod::TransparentOD) > 0) {
			fBuffer->fill([this, &copiedDepth]() {
				if (!copiedDepth) {
					glClear(GL_DEPTH_BUFFER_BIT);
					fBuffer->copyDepth(gBuffer);
				}

				scene->drawTransparentOD();
			});
		}

		//Order-independent Transparent pass
		if (tBuffer != nullptr && scene->count(ShadingMethod::TransparentOID) > 0) {
			tBuffer->fill([this]() {
				glClear(GL_DEPTH_BUFFER_BIT);
				tBuffer->copyDepth(gBuffer);
				scene->drawTransparentOID();
			});

			tBuffer->composite();
		}

		
		glDisable(GL_DEPTH_TEST);

		//Post processing
		//Draw external post processing effects first.
		for (auto it = externalEffects.begin(); it != externalEffects.end(); it++) {
			PostProcessingEffect& effect = *it->second;
			RenderTexture& texture = *it->first;

			stackBuffer.push(texture);
			effect.fill();
		}

		//Draw single effect to screen (if wanted)
		if (isolatedEffect != nullptr) {
			PostProcessingEffect* def = effects.front().second;

			//Save regular rendering settings
			bool onlyEffect = isolatedEffect->getEffectOnly();
			const Texture& buffer = isolatedEffect->getImage();

			//Draw isolated effect
			isolatedEffect->effectOnly(true);
			isolatedEffect->setImage(*texture1);

			stackBuffer.push(*texture2);
			isolatedEffect->fill();
				
			def->draw();

			//Restore render settings
			isolatedEffect->effectOnly(onlyEffect);
			isolatedEffect->setImage(buffer);
		}
		//Draw all included post effects to screen
		else {
			//Draw all the post processing effects on top of each other. (Skip default effect)
			for (auto it = next(effects.begin()); it != effects.end(); it++) {
				PostProcessingEffect& effect = *it->second;
				RenderTexture& texture = *it->first;

				stackBuffer.push(texture);
				effect.fill();
			}

			//Draw the last (default) effect to screen.
			PostProcessingEffect& def = *effects.front().second;
			def.draw();
		}

		scene->unlock();

		//Render GUI overlay on top of final image
		if (gui != nullptr) gui->draw();


		window->swapBuffer();
	}

	void DeferredRenderer::draw() {
		glEnable(GL_DEPTH_TEST);

		//Geometry pass
		gBuffer.fill(geometryPassFunc);

		scene->getLightmanager().update(*scene, &scene->getCamera());

		//SSAO pass
		if (ssao != nullptr) {
			stackBuffer.push(*ssaoTexture);
			stackBuffer.fill(*ssao);
			FrameBuffer::resetSize(window->resolution);
		}

		const IFrameBuffer* parent = getParentBuffer();
		if (parent != nullptr)
			parent->bind();
		else
			throw "No parent for deferred renderer set\n";

		getParentBuffer()->bind();
		lightingPass();
		scene->drawSkybox();
	}

	void DeferredRenderer::draw(const Camera& camera, const FrameBuffer& buffer) {
		glEnable(GL_DEPTH_TEST);

		//Geometry pass
		gBuffer.fill([this, &camera] () { scene->drawDefault(camera); });

		scene->getLightmanager().update(*scene, nullptr);

		//SSAO pass
		if (ssao != nullptr) {
			ssao->setCamera(camera);
			stackBuffer.push(*ssaoTexture);
			stackBuffer.fill(*ssao);
			ssao->updateCamera(scene->getCamera());
		}

		buffer.resetSize();
		buffer.bind();

		//Draw lighting pass and skybox directly into given framebuffer
		lightingPass(camera);
		scene->drawSkybox(camera);
	}

	void DeferredRenderer::drawForward(const Camera& camera) {
		glEnable(GL_DEPTH_TEST);
		scene->drawDefaultForward(camera);
	}

	
	void DeferredRenderer::geometryPass() {
		scene->updateCamera();
		scene->drawDefault();
	}

	void DeferredRenderer::lightingPass() {
		lighting.draw();
	}

	void DeferredRenderer::lightingPass(const Camera& camera) {
		lighting.setCamera(camera);
		lighting.draw();
		lighting.updateCamera(scene->getCamera());
	}


	void DeferredRenderer::forwardPass() {
		glClear(GL_DEPTH_BUFFER_BIT);
		//Copy depth buffer from gBuffer to draw forward 
		//rendered objects 'into' the scene instead of 'on top'
		stackBuffer.copyDepth(gBuffer);

		//Generic pass
		scene->drawGeneric();
		scene->drawSkybox();
	}

	void DeferredRenderer::addSSAO(SSAO& ssao) {
		this->ssao = &ssao;
		addRequester(*this->ssao);

		Resolution ssaoRes = Resolution(window->resolution, ssao.getResolution());
		ssaoTexture = new RenderTexture(ssaoRes, ColorType::Single, WrapMode::ClampEdge, FilterMode::None);

		ssao.init(ScreenQuad::get(), stackBuffer, ssaoRes);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect) {
		SSAO* ssao = dynamic_cast<SSAO*>(&effect);
		if (ssao != nullptr) {
			addSSAO(*ssao);
			return;
		}

		linkImageBuffer(effect);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect, RenderTexture& texture) {
		linkImageBuffer(effect, &texture);
	}


	void DeferredRenderer::addRenderTexture(DynamicRenderTexture& texture) {
		renderTextures.push_back(&texture);
	}

	void DeferredRenderer::addRequester(WorldMapRequester& requester) {
		this->requester.push_back(&requester);
	}


	void DeferredRenderer::linkInformation() const {
		//Link world maps to requesting post effects
		map<WorldMaps, const Texture*> worldMaps = std::move(getMaps());

		for (auto it = requester.begin(); it != requester.end(); it++) {
			WorldMapRequester* req = *it;
			req->addWorldInformation(worldMaps);
		}
	}

	map<WorldMaps, const Texture*> DeferredRenderer::getMaps() const {
		map<WorldMaps, const Texture*> worldMaps;
		worldMaps[WorldMaps::Image] = texture1;
		worldMaps[WorldMaps::Diffuse] = &gBuffer.getDiffuse();
		worldMaps[WorldMaps::PositionRoughness] = &gBuffer.getPositionRoughness();
		worldMaps[WorldMaps::NormalMetallic] = &gBuffer.getNormalMetallic();

		const RenderTexture* emissivity = &gBuffer.getEmissivity();
		if (!emissivity->isEmpty())
			worldMaps[WorldMaps::Emissivity] = emissivity;

		if (ssao != nullptr)
			worldMaps[WorldMaps::SSAO] = ssaoTexture;

		return worldMaps;
	}

	void DeferredRenderer::addFBuffer(ForwardBuffer& buffer) {
		fBuffer = &buffer;
		fBuffer->init(*texture1);
	}

	void DeferredRenderer::addTBuffer(TransparentOIDBuffer& buffer) {
		tBuffer = &buffer;
		tBuffer->init(*texture1);
	}

	StackBuffer& DeferredRenderer::getStackbuffer() {
		return stackBuffer;
	}


	void DeferredRenderer::linkImageBuffer(PostProcessingEffect& effect, RenderTexture* texture) {
		if (texture != nullptr)
			externalEffects.push_back(PostEffectRender(texture, &effect));
		else {
			//Init all post processing effects with two alternating textures
			//Current effect will then always read from one and write to the other
			const Texture& source = (effects.size() % 2 == 0) ? *texture2 : *texture1;
			RenderTexture& target = (effects.size() % 2 == 0) ? *texture1 : *texture2;

			effects.push_back(PostEffectRender(&target, &effect));
			effect.setImage(source);
		}
		
		effect.init(ScreenQuad::get(), stackBuffer, window->resolution);
	}

	void DeferredRenderer::initDefaultEffect() {
		//Link framebuffer of last added post processing effect to default effect
		const Texture& buffer = (effects.size() % 2 == 0) ? *texture2 : *texture1;

		PostProcessingEffect& def = *effects.front().second;
		def.setImage(buffer);
		def.init(ScreenQuad::get(), stackBuffer, window->resolution);
	}


	void DeferredRenderer::handleInput(int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
			toggleBuffer(false);
		
		else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
			toggleBuffer(true);

	}

	void DeferredRenderer::toggleBuffer(bool next) {
		PostProcessingEffect& def = *effects.front().second;
		std::vector<const Texture*> buffers = { &def.getImage(), 
			&gBuffer.getDiffuse(), &gBuffer.getNormalMetallic() };

		const RenderTexture& emisTex = gBuffer.getEmissivity();
		if (!emisTex.isEmpty())
			buffers.push_back(&emisTex);
		if (ssao != nullptr)
			buffers.push_back(ssaoTexture);

		int max = int(buffers.size() + effects.size());
		int i = next ? 1 : -1;
		toggle = abs((toggle + i) % max);

		const Texture* currBuffer = buffers[0];
		if (toggle < buffers.size()) {
			currBuffer = buffers[toggle];

			isolatedEffect = nullptr;
		}
		else {
			currBuffer = texture2;

			int index = toggle - buffers.size();
			isolatedEffect = effects[index].second;
		}

		def.setImage(*currBuffer);
		
	}

}