#pragma once

#include "../application/configuration.h"

using namespace geeL;


class CapsuleScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Invasion", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(-4.8f, -1.3f, 7.45f), vec3(-101.f, -35.f, -2.f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 65.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(5.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/Arches_E_PineTree_3k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 240.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(0.5f, -2.9f, 1.4f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Huge, 6.f, 15U, 150.f);
			lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config);

			lightIntensity = 1.5f;
			Transform& lightTransform21 = transformFactory.CreateTransform(vec3(-5.8f, -0.2f, -3.6f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::High, 6.f, 15U, 150.f, 1.f);
			lightManager.addPointLight(lightTransform21, glm::vec3(lightIntensity * 3.f, lightIntensity * 59.f, lightIntensity * 43.f), config2);

			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(0.1f));
			MeshRenderer& capsule = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/capsule/Capsule.obj"),
				meshTransform2, CullingMode::cullFront, "Capsule");
			scene.addMeshRenderer(capsule);

			capsule.iterateMaterials([&](MaterialContainer& container) {
				container.setIntValue("InverseRoughness", 1);
			});

			Transform& meshTransform1 = transformFactory.CreateTransform(vec3(0.0f, -5.f, 4.4f), vec3(0.f, 0.f, 0.f), vec3(0.12f));
			MeshRenderer& nano = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/nanosuit/nanosuit.obj"),
				meshTransform1, CullingMode::cullFront, "Nano");
			scene.addMeshRenderer(nano);


			ObjectLister& objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			gui.addElement(postLister);
			gui.addSystemInformation(0.01f, 0.74f, 0.17f, 0.145f);


			def.setExposure(3.5f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(4.f, 0.3f);
			SSAO& ssao = SSAO(blur, 2.f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			GenericPostSnippet& iblSnippet = GenericPostSnippet(ibl);
			renderer.addEffect(ibl, ibl);
			postLister.add(iblSnippet);

			BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
			GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 100);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, ResolutionPreset::TWENTY, ResolutionPreset::TWENTY);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth, DrawTime::Late);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);


			DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.4f, 155.f);
			DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, camera.depth, 25.f, 
				camera.getFarPlane(), ResolutionPreset::FULLSCREEN);
			//renderer.addEffect(dof, dof);
			//postLister.add(dof);

			ColorCorrection& colorCorrect = ColorCorrection();
			colorCorrect.setDistortionDirection(glm::vec2(1.f, 0.f));
			colorCorrect.setChromaticDistortion(glm::vec3(0.001f, 0.f, 0.f));
			renderer.addEffect(colorCorrect, DrawTime::Late);
			postLister.add(colorCorrect);

			FXAA& fxaa = FXAA(0.001f, 0.f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, GBufferContent::Default, PhysicsType::World);
		config.run();
	}

};
