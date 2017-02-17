#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include "../utility/rendertime.h"
#include "../shader/shader.h"
#include "../postprocessing/drawdefault.h"
#include "../texturing/simpletexture.h"
#include "../window.h"
#include "../scripting/scenecontrolobject.h"
#include "../inputmanager.h"
#include "../postprocessing/ssao.h"
#include "../postprocessing/postprocessing.h"
#include "../postprocessing/worldpostprocessing.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
#include "../shader/shadermanager.h"
#include "../transformation/transform.h"
#include "../scene.h"
#include "deferredrenderer.h"

#define fps 10

using namespace std;

namespace geeL {

	DeferredRenderer::DeferredRenderer(RenderWindow* window, InputManager* inputManager) 
		: DeferredRenderer(window, inputManager, nullptr) {}

	DeferredRenderer::DeferredRenderer(RenderWindow* window, InputManager* inputManager, SSAO* ssao, float ssaoResolution)
		:
		Renderer(window, inputManager), ssao(ssao), frameBuffer1(FrameBuffer()), frameBuffer2(FrameBuffer()),
			gBuffer(GBuffer()), screen(ScreenQuad(window->width, window->height)), ssaoResolution(ssaoResolution),
			deferredShader(new Shader("renderer/shaders/deferredlighting.vert",
				"renderer/shaders/cooktorrancedeferred.frag")) {

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glFrontFace(GL_CW);

			//Default post processing with tone mapping and gamma correction
			DefaultPostProcess* defaultEffect = new DefaultPostProcess();
			defaultEffect->setScreen(screen);
			effects.push_back(defaultEffect);
	}

	DeferredRenderer::~DeferredRenderer() {
		delete effects.front(); //Default post effect
		delete deferredShader;

		if (ssaoBuffer != nullptr)
			delete ssaoBuffer;

		if (ssaoScreen != nullptr)
			delete ssaoScreen;
	}


	void exitCallbackkk(GLFWwindow* window, int key, int scancode, int action, int mode) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void DeferredRenderer::init() {
		inputManager->addCallback(exitCallbackkk);
		inputManager->init(window);

		gBuffer.init(window->width, window->height);

		if (ssao != nullptr) {
			ssaoBuffer = new FrameBuffer();
			ssaoBuffer->init(window->width * ssaoResolution, window->height * ssaoResolution, 
				1, Single, GL_NEAREST, false);
		}

		deferredShader->use();
		deferredShader->mapOffset = 1;
		deferredShader->addMap(gBuffer.positionDepth, "gPositionDepth");
		deferredShader->addMap(gBuffer.normalMet, "gNormalMet");
		deferredShader->addMap(gBuffer.diffuseSpec, "gDiffuseSpec");
		
		if (ssao != nullptr) {
			deferredShader->addMap(ssaoBuffer->getColorID(), "ssao");
			deferredShader->setInteger("useSSAO", 1);
		}

		frameBuffer1.init(window->width, window->height, 2, { RGBA16, Single });
		frameBuffer2.init(window->width, window->height, 2, { RGBA16, Single });
		screen.init();
	}

	void DeferredRenderer::render() {

		deferredShader->bindMaps();

		//Set color buffer of default effect depending on the amount of added effects
		if (effects.size() % 2 == 0)
			effects.front()->setBuffer(frameBuffer2.getColorID());
		else
			effects.front()->setBuffer(frameBuffer1.getColorID());

		//Init SSAO (if added)
		if (ssao != nullptr) {
			linkWorldInformation(*ssao);
			ssao->setParentFBO(ssaoBuffer->fbo);

			ssaoScreen = new ScreenQuad(screen.width * ssaoResolution, screen.height * ssaoResolution);
			ssaoScreen->init();

			ssao->setScreen(*ssaoScreen);
		}

		shaderManager->staticDeferredBind(*scene, *deferredShader);
		shaderManager->staticBind(*scene);

		//Render loop
		while (!window->shouldClose()) {
			int currFPS = ceil(Time::deltaTime * 1000.f);
			this_thread::sleep_for(chrono::milliseconds(fps - currFPS));
			glfwPollEvents();
			inputManager->update();
			handleInput();

			glEnable(GL_DEPTH_TEST);
			scene->lightManager.drawShadowmaps(*scene);

			//Geometry pass
			gBuffer.fill(*this);

			//Hacky: Read camera depth from geometry pass and write it into the scene
			scene->setCameraDepth(gBuffer.getDepth());

			//SSAO pass
			if (ssao != nullptr) {
				ssaoBuffer->fill(*ssao);
				FrameBuffer::resetSize(screen.width, screen.height);
			}

			//Lighting & forward pass
			frameBuffer1.fill(*this);

			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			//Post processing
			bool chooseBuffer = true;
			int counter = 0;
			//Draw all the post processing effects on top of each other. Ping pong style!
			for (auto effect = next(effects.begin()); effect != effects.end(); effect++) {
				if (chooseBuffer)
					frameBuffer2.fill(**effect);
				else
					frameBuffer1.fill(**effect);

				chooseBuffer = !chooseBuffer;
				counter++;
			}
			//Draw the last (default) effect to screen.
			effects.front()->draw();
			
			window->swapBuffer();
			Time::update();
		}

		for (size_t i = 0; i < objects.size(); i++)
			objects[i]->quit();

		window->close();
	}

	void DeferredRenderer::draw() {
		if (geometryPass) {
			for (size_t i = 0; i < objects.size(); i++)
				objects[i]->draw(scene->camera);

			scene->update();
			shaderManager->bindCamera(*scene);
			scene->drawDeferred();
		}
		else {
			//glClear(GL_COLOR_BUFFER_BIT);
			//Lighting pass
			deferredShader->use();
			deferredShader->loadMaps();
			scene->lightManager.deferredBind(*scene, *deferredShader);
			deferredShader->setMat4("inverseView", glm::inverse(scene->camera.getViewMatrix()));
			deferredShader->setVector3("origin", scene->GetOriginInViewSpace());
			deferredShader->setVector3("ambient", scene->lightManager.ambient);
			screen.draw();

			glClear(GL_DEPTH_BUFFER_BIT);
			//Copy depth buffer from gBuffer to draw forward 
			//rendered objects 'into' the scene instead of 'on top'
			frameBuffer1.copyDepth(gBuffer.fbo);

			//Forward pass
			shaderManager->dynamicBind(*scene);
			scene->drawForward();
			scene->drawSkybox();
		}

		geometryPass = !geometryPass;
	}

	void DeferredRenderer::handleInput() {
		scene->camera.handleInput(*inputManager);
	}

	void DeferredRenderer::setScene(RenderScene& scene) {
		Renderer::setScene(scene);

		deferredShader->addMap(scene.getSkyboxID(), "skybox", GL_TEXTURE_CUBE_MAP);
	}

	void DeferredRenderer::addEffect(PostProcessingEffect& effect) {
		effect.setScreen(screen);
		effects.push_back(&effect);

		//Init all post processing effects with two alternating framebuffers
		//Current effect will then always read from one and write to the other
		if (effects.size() % 2 == 0)
			effect.setBuffer(frameBuffer1.getColorID());
		else
			effect.setBuffer(frameBuffer2.getColorID());
	}

	void DeferredRenderer::addEffect(WorldPostProcessingEffect& effect) {
		effects.push_back(&effect);

		linkWorldInformation(effect);
		effect.setScreen(screen);
	}

	void DeferredRenderer::linkWorldInformation(WorldPostProcessingEffect& effect) {
		auto maps     = list<unsigned int>();
		auto matrices = list<mat4>();
		auto vectors  = list<vec3>();

		auto requiredMaps = effect.requiredWorldMapsList();
		for (auto it = requiredMaps.begin(); it != requiredMaps.end(); it++) {
			switch (*it) {
				case WorldMaps::RenderedImage:
					if (effects.size() % 2 == 0)
						maps.push_back(frameBuffer1.getColorID());
					else
						maps.push_back(frameBuffer2.getColorID());
					break;
				case WorldMaps::DiffuseRoughness:
					maps.push_back(gBuffer.diffuseSpec);
					break;
				case WorldMaps::PositionDepth:
					maps.push_back(gBuffer.positionDepth);
					break;
				case WorldMaps::NormalMetallic:
					maps.push_back(gBuffer.normalMet);
					break;
				case WorldMaps::Specular:
					if (effects.size() % 2 == 0)
						maps.push_back(frameBuffer1.getColorID(1));
					else
						maps.push_back(frameBuffer2.getColorID(1));
					break;
			}
		}

		auto requiredMatrices = effect.requiredWorldMatricesList();
		for (auto it = requiredMatrices.begin(); it != requiredMatrices.end(); it++) {
			switch (*it) {
				case WorldMatrices::View:
					matrices.push_back(scene->camera.getViewMatrix());
					break;
				case WorldMatrices::Projection:
					matrices.push_back(scene->camera.getViewMatrix());
					break;
			}
		}

		auto requiredVectors = effect.requiredWorldVectorsList();
		for (auto it = requiredVectors.begin(); it != requiredVectors.end(); it++) {
			switch (*it) {
				case WorldVectors::CameraPosition:
					vectors.push_back(scene->camera.getPosition());
					break;
				case WorldVectors::CameraDirection:
					vectors.push_back(scene->camera.getDirection());
					break;
				case WorldVectors::OriginView:
					vectors.push_back(scene->GetOriginInViewSpace());
					break;
			}
		}

		effect.addWorldInformation(maps, matrices, vectors);
	}

}