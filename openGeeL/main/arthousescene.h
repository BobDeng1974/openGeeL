#pragma once

#include "../application/configuration.h"

using namespace geeL;


class ArthouseScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Art Studio", Resolution(1920, 1080), WindowMode::Windowed);

		Transform& cameraTransform = Transform(vec3(2.93f, 0.71f, -0.59f), vec3(90.f, 76.86f, 179.f), vec3(1.f));
		PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 5.f, 0.65f, 45.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);

		auto init = [&window, &camera](Application& app, DeferredRenderer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {

			Transform& probeTransform = transformFactory.CreateTransform(vec3(-6.9f, 1.9f, 2.3f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
			DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024, 20, 20, 20);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv1/Playa_Sunrise.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(probe);


			float lightIntensity = 10.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-0.9f, 1.9f, 0.4f), vec3(-180.0f, 0, -50), vec3(1.f, 1.f, 1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 5.f, 20);
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config);

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			ImageTexture& texture = materialFactory.CreateTexture("resources/textures/cookie.png",
				ColorType::GammaSpace, WrapMode::ClampBorder, FilterMode::Linear);

			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-14.88f, 0.4f, -1.88f), vec3(90.f, -56.24f, 179.f), vec3(1.f, 1.f, 1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.00001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			SpotLight& spotLight = lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle, config2);
			spotLight.setLightCookie(texture);

			float scale = 0.008f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(scale));
			MeshRenderer& studio = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/art/artStudio.obj"),
				meshTransform2, CullingMode::cullFront, "Studio");
			scene.addMeshRenderer(studio);


			ObjectLister& objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			gui.addElement(postLister);
			SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
			gui.addElement(sysInfo);

			def.setExposure(2.f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(1.5f, 0.7f);
			SSAO& ssao = SSAO(blur, 2.f);
			renderer.addSSAO(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl, ibl);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.6f, 0.6f);
			renderer.addEffect(ssrrSmooth, ssrr);
			scene.addRequester(ssrr);
			SSRRSnippet& ssrrSnippet = SSRRSnippet(ssrr);
			GaussianBlurSnippet& blurSnippet = GaussianBlurSnippet(blur4);
			postLister.add(ssrrSmooth, ssrrSnippet, blurSnippet);

			BrightnessFilterCutoff& bFilter = BrightnessFilterCutoff(0.7f);
			GaussianBlur& bBlur = GaussianBlur(KernelSize::Huge, 10.1f);
			BlurredPostEffect& bBlurred = BlurredPostEffect(bFilter, bBlur, 0.5f, 0.6f);
			bBlurred.effectOnly(true);
			LensFlare& lensFlare = LensFlare(bBlurred, 0.35f, 6.f);
			lensFlare.setDistortion(glm::vec3(0.04f, 0.03f, 0.02f));
			lensFlare.setStrength(0.3f);
			ImageTexture& dirtTexture = ImageTexture("resources/textures/lens_dirt.jpg", ColorType::GammaSpace);
			//lensFlare.setDirtTexture(dirtTexture);
			ImageTexture& starTexture = ImageTexture("resources/textures/starburst3.jpg", ColorType::GammaSpace, WrapMode::ClampEdge, FilterMode::Linear);
			lensFlare.setStarburstTexture(starTexture);
			LensFlareSnippet& lensSnippet = LensFlareSnippet(lensFlare);
			renderer.addEffect(lensFlare);
			scene.addRequester(lensFlare);
			postLister.add(lensSnippet);

			SobelFilter& sobel = SobelFilter(5.f);
			SobelBlur& sobelBlur = SobelBlur(sobel, 15.f);
			VolumetricLight& vol = VolumetricLight(spotLight, 0.02f, 1.f, 150);
			BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, 0.4f, 0.5f);
			VolumetricLightSnippet& lightSnippet = VolumetricLightSnippet(vol);
			SobelBlurSnippet& snip = SobelBlurSnippet(sobelBlur);
			renderer.addEffect(volSmooth, { &vol, &sobelBlur });
			scene.addRequester(vol);
			postLister.add(volSmooth, lightSnippet, snip);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa);
			postLister.add(fxaa);
			

			app.run();
		};


		Configuration config(window, camera, init);
		config.run();
	}

};