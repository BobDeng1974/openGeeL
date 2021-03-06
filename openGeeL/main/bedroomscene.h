#pragma once

#include "../application/configuration.h"

using namespace geeL;


class BedroomScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Bedroom", Resolution(1920, 1080), WindowMode::Windowed);

		
		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(7.36f, 5.55f, -4.4f), vec3(86.3f, 55.65f, -179.44f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(5.f, 0.45f);
			scene.setCamera(camera);

			Transform& probeTransform = transformFactory.CreateTransform(vec3(0.5f, 7.1f, 5.5f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
			float probeSize = 12.f;
			DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024, probeSize, probeSize, probeSize);

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/Tropical_Beach_3k.hdr", 
				cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(probe);

			float lightIntensity = 50.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(0.01f, 9.4f, -0.1f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 8.f, 2);
			PointLight& gr = lightManager.addPointLight(config, lightTransform1, glm::vec3(lightIntensity * 0.69, lightIntensity * 0.32, lightIntensity * 0.22));

			lightIntensity = 100.f;
			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-31.4f, 18.79f, -9.7f), vec3(119.4f, 58.5f, 2.9f), vec3(2.9f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.0001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			SpotLight& spotLight = lightManager.addSpotlight(config2, lightTransform2, glm::vec3(lightIntensity * 0.85f, lightIntensity * 0.87f, lightIntensity * 0.66f), 25.5f, 2.f);

			float scale = 0.05f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(scale));
			StaticModelRenderer bedroomScene = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/bedroom/Bedroom2.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform2, false);

			for (auto it(bedroomScene.begin()); it != bedroomScene.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				MaterialContainer& m = renderer->getMaterial().getMaterialContainer();
				if (m.name == "Bottle") {
					m.setFloatValue("Transparency", 0.3f);

					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);
					renderer->setShader(ss);
				}

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}


			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(4.f);
			def.setAdaptiveExposure(true);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(4.257f, 7, 0.323f);
			SSAO& ssao = SSAO(blur, 4.f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			GenericPostSnippet& iblSnippet = GenericPostSnippet(ibl);
			renderer.addEffect(ibl, DrawTime::Early);
			postLister.add(iblSnippet);

			SSAOSnippet& ssaoSnippet = SSAOSnippet(ssao);
			BilateralFilterSnippet& ssaoBlurSnippet = BilateralFilterSnippet(blur);
			PostGroupSnippet& groupSnippet = PostGroupSnippet(ssaoSnippet, ssaoBlurSnippet);
			postLister.add(groupSnippet);

			SobelFilter& sobel = SobelFilter(5.f);
			SobelBlur& sobelBlur = SobelBlur(sobel, 15.f);
			VolumetricLight& vol = VolumetricLight(spotLight, 0.7f, 14.f, 250);
			BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, ResolutionPreset::TWENTYFIVE, ResolutionPreset::TWENTY);
			VolumetricLightSnippet& lightSnippet = VolumetricLightSnippet(vol);
			SobelBlurSnippet& snip = SobelBlurSnippet(sobelBlur);
			renderer.addEffect(volSmooth);
			scene.addRequester(vol);
			postLister.add(volSmooth, lightSnippet, snip);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			//MultisampledSSRR& ssrr = MultisampledSSRR();
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, ResolutionPreset::HALFSCREEN, ResolutionPreset::HALFSCREEN);
			renderer.addEffect(ssrrSmooth);
			scene.addRequester(ssrr);
			SSRRSnippet& ssrrSnippet = SSRRSnippet(ssrr);
			postLister.add(ssrrSmooth, ssrrSnippet);

			FXAA& fxaa = FXAA(0.f, 0.f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, TonemappingMethod::ACESFilm);
		config.run();
	}

};