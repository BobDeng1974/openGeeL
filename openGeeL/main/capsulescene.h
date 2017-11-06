#pragma once

#include "../application/configuration.h"

using namespace geeL;


class CapsuleScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("geeL", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(3.0f, 0.3f, 8.25f), vec3(-106.f, 22.f, 0.f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(5.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/Arches_E_PineTree_3k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 40.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-0.5f, -2.9f, 3), vec3(-180.0f, 0, -50), vec3(1.f), true);
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


			Transform& meshTransform22 = transformFactory.CreateTransform(vec3(0.0f, -5.25f, 5.9f), vec3(0.f, 0.f, 0.f), vec3(0.12f));
			MeshRenderer& girl = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/girl/girl_complete_03.obj"),
				meshTransform22, CullingMode::cullFront, "Girl");
			scene.addMeshRenderer(girl);

			/*
			girl.iterateMeshes([&](const Mesh& mesh) {
				if (mesh.getName() == "g eyelash" || mesh.getName() == "g hair_outer" 
					|| mesh.getName() == "g hair_inner" || mesh.getName() == "g fur") {
					
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Forward);
					girl.changeMaterial(ss, mesh);
				}
			});
			*/

			girl.iterateMaterials([&](MaterialContainer& container) {
				if (container.name == "fur")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/girl/fur_alpha_02.jpg"));
				else if (container.name == "eyelash")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/girl/eyelash_alpha_01.jpg"));
				else if (container.name == "hair_inner")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/girl/hair_inner_alpha_01.jpg"));
				else if (container.name == "hair_outer")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/girl/hair_outer_alpha_01.jpg"));
				else if (container.name == "cloth") {
					container.addTexture("emission", materialFactory.CreateTexture("resources/girl/cloth_spec_01.jpg", ColorType::GammaSpace));
					container.setVectorValue("Emissivity", vec3(50.f));
					container.setFloatValue("Roughness", 0.5f);
					container.setFloatValue("Metallic", 1.f);
				}
				else if (container.name == "light")
					container.setVectorValue("Emissivity", vec3(100.f));
				else if (container.name == "body")
					container.setFloatValue("Roughness", 0.7f);

			});


			ObjectLister& objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			gui.addElement(postLister);
			SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
			gui.addElement(sysInfo);


			def.setExposure(2.5f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(1.8f, 0.7f);
			SSAO& ssao = SSAO(blur, 0.5f);
			//renderer.addEffect(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl, ibl);

			BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
			GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 100);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth, DrawTime::Late);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);


			MotionBlur& motionBlur = MotionBlur(0.3f, 20);
			MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
			renderer.addEffect(motionBlur, DrawTime::Late);
			scene.addRequester(motionBlur);
			postLister.add(mSnippet);

			DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.4f, 155.f);
			DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, camera.depth, 25.f, camera.getFarPlane(), 1.f);
			//renderer.addEffect(dof, dof);
			//postLister.add(dof);

			BrightnessFilterCutoff& filter = BrightnessFilterCutoff(1.f);
			GaussianBlur& bloomBlur = GaussianBlur(KernelSize::Large, 4.f);
			Bloom& bloom = Bloom(filter, bloomBlur, 1.f, 0.7f);
			renderer.addEffect(bloom, DrawTime::Late);
			postLister.add(bloom);

			GaussianBlurSnippet snipsnip(bloomBlur);
			postLister.add(snipsnip);

			ColorCorrection& colorCorrect = ColorCorrection();
			renderer.addEffect(colorCorrect, DrawTime::Late);
			postLister.add(colorCorrect);

			FXAA& fxaa = FXAA(0.001f, 0.f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, GBufferContent::DefaultEmissive, PhysicsType::World);
		config.run();
	}

};
