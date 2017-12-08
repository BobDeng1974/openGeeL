#pragma once

#include "shader/sceneshader.h"
#include "../application/configuration.h"

using namespace geeL;


class ModelViewer {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Model Viewer", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(-7.92f, 2.86f, 2.61f), vec3(89.13f, -42.91f, 152.11f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 80.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(3.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/Theatre-Center_2k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(iblMap.getIrradianceMap());
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 85.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(1.7f, 6.9f, 8.2), vec3(180.0f, 0, -50), vec3(1.f), false);
			lightManager.addPointLight(lightTransform1, glm::vec3(225.f / 256.f, 203.f / 256.f, 198.f / 256.f) * lightIntensity, noShadowMapConfig);

			lightIntensity = 69.f;
			Transform& lightTransform21 = transformFactory.CreateTransform(vec3(-5.2f, -0.2f, 7.5f), vec3(-180.0f, 0, -50), vec3(1.f), false);
			ShadowMapConfiguration& config = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Medium, 3.f, 15U, 150.f);
			lightManager.addPointLight(lightTransform21, glm::vec3(13.f / 256.f, 255.f / 256.f, 186.f / 256.f) * lightIntensity, config);

			Transform& meshTransform3 = transformFactory.CreateTransform(vec3(1.5f, 0.34f, 12.5f), vec3(180.f, 29.6f, 180.f), vec3(0.12f));
			SkinnedMeshRenderer& skull = meshFactory.CreateMeshRenderer(meshFactory.CreateSkinnedModel("resources/skull/skull.fbx"),
				meshTransform3, "Skull");
			scene.addMeshRenderer(skull);

			SimpleAnimator& anim = skull.addComponent<SimpleAnimator>(skull.getSkinnedModel(), skull.getSkeleton());
			anim.loopAnimation(true);
			anim.startAnimation("AnimStack::Take 001");

			skull.iterateMaterials([&](MaterialContainer& container) {
				if (container.name == "skullhull") {
					container.addTexture("diffuse", materialFactory.CreateTexture("resources/skull/Servoskull_mechanics_diff2.jpg", ColorType::GammaSpace, FilterMode::Bilinear));
					container.addTexture("normal", materialFactory.CreateTexture("resources/skull/Servoskull_mechanics_normal.jpg", ColorType::RGBA, FilterMode::Bilinear));
					container.addTexture("roughness", materialFactory.CreateTexture("resources/skull/Servoskull_mechanics_gloss.jpg", ColorType::RGBA, FilterMode::Bilinear));

					container.setFloatValue("Roughness", 0.45f);
					container.setFloatValue("Metallic", 0.95f);
					container.setVectorValue("Color", vec3(0.2f));
				}
				else if (container.name == "skullface") {
					container.addTexture("diffuse", materialFactory.CreateTexture("resources/skull/Servoskull_face_diff.jpg", ColorType::GammaSpace, FilterMode::Bilinear));
					container.addTexture("normal", materialFactory.CreateTexture("resources/skull/Servoskull_face_normal.jpg", ColorType::RGBA, FilterMode::Bilinear));
					container.addTexture("specular", materialFactory.CreateTexture("resources/skull/Servoskull_face_gloss.jpg", ColorType::RGBA, FilterMode::Bilinear));

					container.setFloatValue("Roughness", 1.f);
					container.setFloatValue("Metallic", 0.1f);
				}
			});


			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(2.5f);
			postLister.add(def);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			GenericPostSnippet& iblSnippet = GenericPostSnippet(ibl);
			renderer.addEffect(ibl, DrawTime::Early);
			postLister.add(iblSnippet);

			BrightnessFilterCutoff& filter = BrightnessFilterCutoff(1.f);
			GaussianBlur& bloomBlur = GaussianBlur(KernelSize::Large, 3.f);
			Bloom& bloom = Bloom(filter, bloomBlur, ResolutionPreset::HALFSCREEN, ResolutionPreset::HALFSCREEN);
			//renderer.addEffect(bloom, DrawTime::Late);
			//postLister.add(bloom);

			BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
			GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 100);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, ResolutionPreset::TWENTYFIVE, ResolutionPreset::TWENTYFIVE);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth, DrawTime::Late);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);

			FXAA& fxaa = FXAA(0.02f, 0.4f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, GBufferContent::DefaultEmissive, PhysicsType::World);
		config.run();
	}

};


