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


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(5.4f, 10.0f, -2.9f), vec3(70.f, 50.f, -175.f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(5.f, 0.45f);
			scene.setCamera(camera);

			Transform& probeTransform = transformFactory.CreateTransform(vec3(0.5f, 7.1f, 5.5f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
			float probeSize = 12.f;
			DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024, probeSize, probeSize, probeSize);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/Tropical_Beach_3k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(probe);

			float lightIntensity = 50.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(0.01f, 9.4f, -0.1f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 7.f, 10);
			PointLight& gr = lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity * 0.69, lightIntensity * 0.32, lightIntensity * 0.22), config);

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-31.4f, 18.79f, -9.7f), vec3(119.4f, 58.5f, 2.9f), vec3(2.9f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.0001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			SpotLight& spotLight = lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity * 0.85f, lightIntensity * 0.87f, lightIntensity * 0.66f), angle, outerAngle, config2);

			float scale = 0.05f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(scale));
			MeshRenderer& bedroom = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/bedroom/Bedroom2.obj"),
				meshTransform2, "Bedroom");
			scene.addMeshRenderer(bedroom);

			std::map<const MeshInstance*, const Material*> transObjects;
			bedroom.iterate([&](const MeshInstance& mesh, const Material& material) {
				if (mesh.getName() == "Soda_Bottle") {
					transObjects[&mesh] = &material;
				}

			});

			for (auto it(transObjects.begin()); it != transObjects.end(); it++) {
				const MeshInstance& mesh = *it->first;
				const Material& material = *it->second;

				MaterialContainer& container = material.getMaterialContainer();
				container.setFloatValue("Transparency", 0.2f);

				SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::TransparentOD);
				bedroom.changeMaterial(ss, mesh);
			}

			transObjects.clear();


			ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			gui.addElement(postLister);
			gui.addSystemInformation(0.01f, 0.74f, 0.17f, 0.145f);

			def.setExposure(15.f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(4.257f, 0.323f);
			SSAO& ssao = SSAO(blur, 4.f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			GenericPostSnippet& iblSnippet = GenericPostSnippet(ibl);
			renderer.addEffect(ibl, DrawTime::Early);
			postLister.add(iblSnippet);

			ImageBasedLighting& forwardIBL = ImageBasedLighting(scene);
			renderer.addEffect(forwardIBL, DrawTime::Intermediate);
			forwardIBL.setRenderMask(RenderMask::Transparent);

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


		Configuration config(window, init);
		config.run();
	}

};