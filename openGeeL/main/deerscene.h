#pragma once

#include "../application/configuration.h"

using namespace geeL;
using namespace std;


class DeerScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Deer", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {

			Transform& cameraTransform = transformFactory.CreateTransform(vec3(-0.03f, 0.17f, 2.66f), vec3(-91.59f, 2.78f, -3.f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 25.f, window.getWidth(), window.getHeight(), 0.01f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(1.f, 0.45f));
			scene.setCamera(camera);

			Transform& probeTransform = transformFactory.CreateTransform(vec3(-0.13f, 0.13f, 0.52f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
			DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024, 20, 20, 20);

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/WinterForest_Ref.hdr", 
				cubeMapFactory.getBuffer(), 1024);
			//IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(probe);


			float lightIntensity = 7.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-0.29f, 0.39f, 1.80f), vec3(-180.0f, 0, -50), vec3(1.f));
			ShadowMapConfiguration config = ShadowMapConfiguration(0.000001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 4.5f, 2);
			&lightManager.addPointLight(config, lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379));

			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f), vec3(0.f), vec3(0.1f, 0.1f, 0.1f));
			StaticModelRenderer deerScene = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/deer/scene2.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform2, false);

			for (auto it(deerScene.begin()); it != deerScene.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				MaterialContainer& m = renderer->getMaterial().getMaterialContainer();
				m.setFloatValue("Metallic", 0.2f);

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}

			/*
			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);
			*/

			BilateralFilter& blur = BilateralFilter(4.f, 7, 0.291f);
			SSAO& ssao = SSAO(blur, 0.5f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);

			def.setExposure(1.6f);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR(75, 0.01f);
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, ResolutionPreset::TWOTHIRDS, ResolutionPreset::TWOTHIRDS);
			renderer.addEffect(ssrrSmooth);
			scene.addRequester(ssrr);

			DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.3f, 10.f);
			DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, 2.f, 45.f, 100.f, ResolutionPreset::FORTY);
			renderer.addEffect(dof);

			ColorCorrection& colorCorrect = ColorCorrection();
			colorCorrect.setContrast(1.05f);
			colorCorrect.setVibrance(0.4f);
			renderer.addEffect(colorCorrect, DrawTime::Late);

			FXAA& fxaa = FXAA(0.f, 0.f);
			renderer.addEffect(fxaa, DrawTime::Late);

			app.run();
		};


		Configuration config(window, init, TonemappingMethod::Uncharted2);
		config.run();
	}

};