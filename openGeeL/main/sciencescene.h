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

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/Playa_Sunrise.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 100.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7.1f, 4.9f, 2.4f), vec3(-180.0f, 0, -50), vec3(1.f, 1.f, 1.f), true);
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), defPLShadowMapConfig);

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			ShadowMapConfiguration config = ShadowMapConfiguration(0.0001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-11, 11, -15), vec3(118.0f, 40, -23), vec3(1.f, 1.f, 1.f), true);
			SpotLight& spotLight = lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle, config);


			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(0.1f, 0.1f, 0.1f));
			MeshRenderer& science = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/mad/madScience.obj"),
				meshTransform2, CullingMode::cullFront, "Science");
			scene.addMeshRenderer(science);

			std::map<const Mesh*, const Material*> transObjects;
			science.iterate([&](const Mesh& mesh, const Material& material) {
				if (mesh.getName() == "GRP_Models_MDL_Glass_GRP_Glass_Triangle" ||
					mesh.getName() == "GRP_Models_GRP_Stack_GRP__MDL_Goggles_Object02" ||
					mesh.getName() == "GRP_Models_GRP_LargeTallBeaker_MDL_Glass1" ||
					mesh.getName() == "GRP_Models_GRP_TallBulbyGlass_MDL_OuterGlass" ||
					mesh.getName().find("Goggles_Obj") != std::string::npos ||
					mesh.getName().find("TestTubes_poly") != std::string::npos ||
					mesh.getName().find("SpoutedBeakerGlass") != std::string::npos ||
					mesh.getName().find("BrainJar_MDL_GlassOuter") != std::string::npos) {

					transObjects[&mesh] = &material;
				}
			});

			for (auto it(transObjects.begin()); it != transObjects.end(); it++) {
				const Mesh& mesh = *it->first;
				const Material& material = *it->second;

				

				MaterialContainer& container = material.getMaterialContainer();

				if (mesh.getName().find("Goggles") != std::string::npos)
					container.setFloatValue("Transparency", 0.6f);
				else {
					if (mesh.getName().find("TestTubes") != std::string::npos)
						container.setFloatValue("Transparency", 0.75f);
					else 
						container.setFloatValue("Transparency", 0.5f);

					container.setVectorValue("Color", glm::vec3(0.3f));
				}

				SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::TransparentOD);
				science.changeMaterial(ss, mesh);
			}

			transObjects.clear();

			ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			//gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			//gui.addElement(postLister);
			SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
			//gui.addElement(sysInfo);

			def.setExposure(2.f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(1, 0.7f);
			SSAO& ssao = SSAO(blur, 3.f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl, ibl);

			SobelFilter& sobel = SobelFilter(15);
			SobelBlur& sobelBlur = SobelBlur(sobel);
			VolumetricLight& vol = VolumetricLight(spotLight, 0.05f, 6.f, 100);
			BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, 0.3f, 0.3f);
			VolumetricLightSnippet& lightSnippet = VolumetricLightSnippet(vol);
			renderer.addEffect(volSmooth, vol, sobelBlur);
			scene.addRequester(vol);
			postLister.add(volSmooth, lightSnippet);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			ssrr.effectOnly(false);
			//BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.5f, 0.9f);
			renderer.addEffect(ssrr, ssrr);
			scene.addRequester(ssrr);

			DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.3f, 5.f);
			DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, camera.depth, 15.f, 100.f, 0.5f);
			renderer.addEffect(dof, dof);
			postLister.add(dof);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa);

			app.run();
		};


		Configuration config(window, init);
		config.run();
	}

};
