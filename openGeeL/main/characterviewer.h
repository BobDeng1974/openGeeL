#pragma once

#include "shader/sceneshader.h"
#include "../application/configuration.h"

using namespace geeL;


class CharacterViewer {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Character Viewer", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(1.3f, -3.58f, 4.11f), vec3(78.13f, 44.83f, -174.44f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 80.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(3.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/TropicalRuins_3k.hdr", 
				cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(iblMap.getIrradianceMap(), 0.75f);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 27.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-0.5f, -2.9f, 3), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Huge, 5.f, 15U, 150.f);
			lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config);

			lightIntensity = 0.5f;
			Transform& lightTransform21 = transformFactory.CreateTransform(vec3(-5.2f, -0.2f, 7.5f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Huge, 6.f, 15U, 150.f, 1.f);
			lightManager.addPointLight(lightTransform21, glm::vec3(lightIntensity * 3.f, lightIntensity * 59.f, lightIntensity * 43.f), config2);

			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, -5.25f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(100.f, 0.2f, 100.f));
			std::unique_ptr<MeshRenderer> planePtr = meshFactory.createMeshRenderer(
				meshFactory.createStaticModel("resources/primitives/plane.obj"),
				meshTransform2, "Floor");
			MeshRenderer& plane = scene.addMeshRenderer(std::move(planePtr));

		
			plane.iterateMeshesSafe([&](const MeshInstance& mesh) {
				SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Hybrid);
				plane.changeMaterial(ss, mesh);
			});

			plane.iterateMaterials([&](MaterialContainer& container) {
				container.setFloatValue("Roughness", 0.25f);
				container.setFloatValue("Metallic", 0.f);
				container.setFloatValue("Transparency", 0.65f);
			});

			{
				SceneShader& forwardShader = materialFactory.getDefaultShader(ShadingMethod::Hybrid);
				SceneShader& transparentShader = materialFactory.getDefaultShader(ShadingMethod::Transparent);
				
				forwardShader.bind<float>("fogFalloff", 15.f);
				transparentShader.bind<float>("fogFalloff", 15.f);
			}

			Transform& meshTransform22 = transformFactory.CreateTransform(vec3(0.0f, -5.25f, 5.9f), vec3(0.f, 0.f, 0.f), vec3(0.12f));
			std::unique_ptr<MeshRenderer> girlPtr = meshFactory.createMeshRenderer(
				meshFactory.createStaticModel("resources/girl/girl_nofloor.obj"),
				meshTransform22, "Girl");
			MeshRenderer& girl = scene.addMeshRenderer(std::move(girlPtr));

			girl.iterateMeshesSafe([&](const MeshInstance& mesh) {
				if (mesh.getName() == "eyelash" || mesh.getName() == "fur") {
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Forward);
					girl.changeMaterial(ss, mesh);
				}
				else if (mesh.getName() == "body")
					girl.setRenderMask(RenderMask::Skin, mesh);
				else if (mesh.getName() == "hair_inner") {
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent);
					girl.changeMaterial(ss, mesh);
				}
			});

			girl.iterateMaterials([&](MaterialContainer& container) {
				if (container.name == "fur") {
					container.addTexture("alpha", materialFactory.createTexture("resources/girl/fur_alpha_01.jpg"));
					container.setFloatValue("Transparency", 0.4f);
				}
				else if (container.name == "eyelash") {
					container.addTexture("alpha", materialFactory.createTexture("resources/girl/eyelash_alpha_01.jpg"));
					container.setFloatValue("Transparency", 0.7f);
				}
				else if (container.name == "hair_inner") {
					container.addTexture("alpha", materialFactory.createTexture("resources/girl/hair_inner_alpha_01.jpg"));
					container.setFloatValue("Transparency", 0.9f);
				}
				else if (container.name == "eyes_outer")
					container.setFloatValue("Roughness", 0.28f);
				else if (container.name == "hair_outer")
					container.addTexture("alpha", materialFactory.createTexture("resources/girl/hair_outer_alpha_01.jpg"));
				else if (container.name == "cloth") {
					container.addTexture("emission", materialFactory.createTexture("resources/girl/cloth_glow_01.jpg", ColorType::GammaSpace));
					container.addTexture("occlusion", materialFactory.createTexture("resources/girl/cloth_ao_01.jpg", ColorType::GammaSpace));
					container.setVectorValue("Emissivity", vec3(20.f));
					container.setFloatValue("Roughness", 0.5f);
					container.setFloatValue("Metallic", 1.f);
				}
				else if (container.name == "light")
					container.setVectorValue("Emissivity", vec3(100.f));
				else if (container.name == "body") {
					container.addTexture("occlusion", materialFactory.createTexture("resources/girl/body_ao_01.jpg", ColorType::GammaSpace));
					container.setFloatValue("Roughness", 0.5f);

					container.setIntValue("InverseRoughness", true);
				}
			});


			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(2.f);
			postLister.add(def);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			GenericPostSnippet& iblSnippet = GenericPostSnippet(ibl);
			renderer.addEffect(ibl, DrawTime::Early);
			postLister.add(iblSnippet);

			BrightnessFilterCutoff& filter = BrightnessFilterCutoff(1.f);
			GaussianBlur& bloomBlur = GaussianBlur(3.f, 17);
			Bloom& bloom = Bloom(filter, bloomBlur, ResolutionPreset::HALFSCREEN, ResolutionPreset::HALFSCREEN);
			renderer.addEffect(bloom, DrawTime::Late);
			postLister.add(bloom);

			GaussianBlurSnippet snipsnip(bloomBlur);
			postLister.add(snipsnip);

			BilateralFilter& blur2 = BilateralFilter(1, 7, 0.1f);
			GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 20);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, ResolutionPreset::TWENTYFIVE, ResolutionPreset::TWENTYFIVE);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth, DrawTime::Late);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);

			MotionBlur& motionBlur = MotionBlur(0.3f, 20);
			MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
			//renderer.addEffect(motionBlur, DrawTime::Late);
			//scene.addRequester(motionBlur);
			//postLister.add(mSnippet);

			//Fake subsurface scattering with gaussian blur
			SeparatedGaussian& sss = SeparatedGaussian(1.3f, 7, 0.55f);
			sss.setSigmaR(3.4f);
			sss.setSigmaG(1.4f);
			sss.setSigmaB(2.4f);
			SeparatedGaussianSnippet& ssssnip = SeparatedGaussianSnippet(sss);
			AdditiveWrapper& additiveSSS = AdditiveWrapper(sss);
			renderer.addEffect(additiveSSS, DrawTime::Early);
			postLister.add(ssssnip);
			additiveSSS.setRenderMask(RenderMask::Skin);

			ColorCorrection& colorCorrect = ColorCorrection();
			colorCorrect.setContrast(1.2f);
			renderer.addEffect(colorCorrect, DrawTime::Late);
			postLister.add(colorCorrect);

			FXAA& fxaa = FXAA(0.02f, 0.15f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, GBufferContent::DefaultEmissive, PhysicsType::World);
		config.run();
	}

};

