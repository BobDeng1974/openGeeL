#pragma once

#include "../application/configuration.h"

using namespace geeL;


class ScienceScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Science", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(6.95f, 2.53f, 5.5f), vec3(-88.f, 34.f, 0.9f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 65.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(5.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/Playa_Sunrise.hdr", 
				cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 100.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7.1f, 4.9f, 2.4f), vec3(-180.0f, 0, -50), vec3(1.f, 1.f, 1.f), true);
			lightManager.addPointLight(defPLShadowMapConfig, lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379));

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			ShadowMapConfiguration config = ShadowMapConfiguration(0.0001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-11, 11, -15), vec3(118.0f, 40, -23), vec3(1.f, 1.f, 1.f), true);
			SpotLight& spotLight = lightManager.addSpotlight(config, lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle);


			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f), vec3(0.f), vec3(0.1f));
			std::list<std::unique_ptr<StaticMeshRenderer>> scienceScene = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/mad/madScience.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform2, false);

			for (auto it(scienceScene.begin()); it != scienceScene.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				const Mesh& mesh = renderer->getMesh();
				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();

				if (mesh.getName() == "GRP_Models_MDL_Glass_GRP_Glass_Triangle" ||
					mesh.getName() == "GRP_Models_GRP_Stack_GRP__MDL_Goggles_Object02" ||
					mesh.getName() == "GRP_Models_GRP_LargeTallBeaker_MDL_Glass1" ||
					mesh.getName() == "GRP_Models_GRP_TallBulbyGlass_MDL_OuterGlass" ||
					mesh.getName().find("Goggles_Obj") != std::string::npos ||
					mesh.getName().find("TestTubes_poly") != std::string::npos ||
					mesh.getName().find("SpoutedBeakerGlass") != std::string::npos ||
					mesh.getName().find("BrainJar_MDL_GlassOuter") != std::string::npos) {

					if (mesh.getName().find("Goggles") != std::string::npos)
						container.setFloatValue("Transparency", 0.6f);
					else {
						if (mesh.getName().find("TestTubes") != std::string::npos)
							container.setFloatValue("Transparency", 0.75f);
						else
							container.setFloatValue("Transparency", 0.5f);

						container.setVectorValue("Color", glm::vec3(0.3f));
					}

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
			//gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(2.f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(5.424f, 7, 0.323f);
			SSAO& ssao = SSAO(blur, 3.f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl);

			SobelFilter& sobel = SobelFilter(15);
			SobelBlur& sobelBlur = SobelBlur(sobel);
			VolumetricLight& vol = VolumetricLight(spotLight, 0.05f, 6.f, 100);
			BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, ResolutionPreset::ONETHIRD, ResolutionPreset::ONETHIRD);
			VolumetricLightSnippet& lightSnippet = VolumetricLightSnippet(vol);
			renderer.addEffect(volSmooth);
			scene.addRequester(vol);
			postLister.add(volSmooth, lightSnippet);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			ssrr.effectOnly(false);
			//BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.5f, 0.9f);
			renderer.addEffect(ssrr);
			scene.addRequester(ssrr);

			DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.3f, 5.f);
			DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, 10.f, 15.f, 100.f, ResolutionPreset::HALFSCREEN);
			renderer.addEffect(dof);
			postLister.add(dof);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa, DrawTime::Late);

			app.run();
		};


		Configuration config(window, init);
		config.run();
	}

};
