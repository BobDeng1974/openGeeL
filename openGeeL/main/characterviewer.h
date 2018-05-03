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

			//Transform& cameraTransform = transformFactory.CreateTransform(vec3(1.3f, -3.58f, 4.11f), vec3(78.13f, 44.83f, -174.44f), vec3(1.f));
			Transform& cameraTransform = transformFactory.CreateTransform(vec3(2.55f, -2.7f, 3.44f), vec3(69.28f, 48.02f, -179.29f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 45.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
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
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Large, 6.f, 1U, 150.f);
			lightManager.addPointLight(config, lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379));

			lightIntensity = 0.5f;
			Transform& lightTransform21 = transformFactory.CreateTransform(vec3(-5.2f, -0.2f, 7.5f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Large, 6.5f, 2U, 150.f, 1.f);
			lightManager.addPointLight(config2, lightTransform21, glm::vec3(lightIntensity * 3.f, lightIntensity * 59.f, lightIntensity * 43.f), 0.25f);



			{
				SceneShader& forwardShader = materialFactory.getDefaultShader(ShadingMethod::Hybrid, false);
				SceneShader& transparentShader = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);

				forwardShader.bind<float>("fadeFalloff", 15.f);
				transparentShader.bind<float>("fadeFalloff", 15.f);
			}


			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, -5.25f, 0.0f), vec3(0.f), vec3(100.f, 0.2f, 100.f));
			StaticModelRenderer plane = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/primitives/plane.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform2, false);

			for (auto it(plane.begin()); it != plane.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Hybrid, false);
				renderer->setShader(ss);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.setFloatValue("Roughness", 0.25f);
				container.setFloatValue("Metallic", 0.f);
				container.setFloatValue("Transparency", 0.65f);

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}



			Transform& meshTransform22 = transformFactory.CreateTransform(vec3(0.0f, -5.25f, 5.9f), vec3(0.f), vec3(0.12f));
			StaticModelRenderer girl = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/girl/girl_nofloor.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform22, false);

			for (auto it(girl.begin()); it != girl.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);
				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();

				const Mesh& mesh = renderer->getMesh();
				if (mesh.getName() == "eyelash" || mesh.getName() == "fur") {
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Forward, false);
					renderer->setShader(ss);
				}
				else if (mesh.getName() == "body")
					renderer->setRenderMask(RenderMask::Skin);
				else if (mesh.getName() == "hair_inner") {
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);
					renderer->setShader(ss);
				}
				else if (mesh.getName() == "tear") {
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);
					renderer->setShader(ss);

					container.setFloatValue("Transparency", 0.2f);
					container.setFloatValue("Roughness", 0.1f);
					container.setFloatValue("Metallic", 0.9f);
				}
				else if (mesh.getName() == "eyes_outer") {
					SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);
					renderer->setShader(ss);

					container.setFloatValue("Transparency", 0.2f);
					container.setFloatValue("Roughness", 0.5f);
					container.setFloatValue("Metallic", 0.5f);
				}


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
				else if (container.name == "eyes_inner")
					container.setFloatValue("Roughness", 0.12f);
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

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}


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
			GaussianBlur& bloomBlur = GaussianBlur(6.f, 25);
			Bloom& bloom = Bloom(filter, bloomBlur, ResolutionPreset::FULLSCREEN, ResolutionPreset::FULLSCREEN);
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

			MotionBlur& motionBlur = MotionBlur(0.3f, 2.f, 10);
			MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
			renderer.addEffect(motionBlur, DrawTime::Late);
			scene.addRequester(motionBlur);
			postLister.add(mSnippet);

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
			colorCorrect.setVibrance(0.25f);
			renderer.addEffect(colorCorrect, DrawTime::Late);
			postLister.add(colorCorrect);

			FXAA& fxaa = FXAA(0.02f, 0.15f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, PhysicsType::World);
		config.run();
	}

};

