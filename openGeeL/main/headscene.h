#pragma once

#include "shader/sceneshader.h"
#include "../application/configuration.h"

using namespace geeL;
using namespace std;


class HeadViewer {

public:
	static void draw() {
		RenderWindow& window = RenderWindow("Head Viewer", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {

			Transform& cameraTransform = transformFactory.CreateTransform(vec3(1.61f, -3.02f, 3.09f), vec3(-101.42f, -78.9f, -1.07f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 45.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(3.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/Alexs_Apt_2k.hdr",
				cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(iblMap.getIrradianceMap(), 0.75f);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 27.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-0.5f, -2.9f, 3), vec3(-180.0f, 0, -50), vec3(1.f), false);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00006f, ShadowMapType::Soft, ShadowmapResolution::Large, 6.f, 1U, 150.f);
			lightManager.addPointLight(noShadowMapConfig, lightTransform1, glm::vec3(lightIntensity *0.925, lightIntensity *0.793, lightIntensity*0.793));

			
			Transform& meshTransform = transformFactory.CreateTransform(vec3(2.29f, -3.15f, 2.99f), vec3(0.f, -38.1f, 0.f), vec3(1.f));
			StaticModelRenderer head = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/head/head.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform, false);


			for (auto it(head.begin()); it != head.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);
				renderer->setRenderMask(RenderMask::SubsurfaceScattering);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.addTexture("diffuse", materialFactory.createTexture("resources/head/Diffuse_Map.jpg", ColorType::GammaSpace));
				container.addTexture("normal", materialFactory.createTexture("resources/head/Normal_Map.jpg", ColorType::RGBA));
				container.addTexture("gloss", materialFactory.createTexture("resources/head/SSS_Map.png", ColorType::RGBA));
				container.addTexture("translucency", materialFactory.createTexture("resources/head/SSS_Map.png", ColorType::RGBA));

				container.setFloatValue("Roughness", 0.6f);

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}
			



			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(1.3f);
			postLister.add(def);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			GenericPostSnippet& iblSnippet = GenericPostSnippet(ibl);
			renderer.addEffect(ibl, DrawTime::Early);
			postLister.add(iblSnippet);
			ibl.setEffectScale(0.4f);


			MotionBlur& motionBlur = MotionBlur(0.2f, 2.f, 15);
			MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
			renderer.addEffect(motionBlur, DrawTime::Late);
			scene.addRequester(motionBlur);
			postLister.add(mSnippet);

			SubsurfaceScattering& sss = SubsurfaceScattering(1.3f, 30, 0.55f);
			sss.setSigmaR(17.4f);
			sss.setSigmaG(7.4f);
			sss.setSigmaB(10.4f);
			SeparatedGaussianSnippet& ssssnip = SeparatedGaussianSnippet(sss);
			AdditiveWrapper& additiveSSS = AdditiveWrapper(sss);
			renderer.addEffect(additiveSSS, DrawTime::Early);
			postLister.add(ssssnip);
			additiveSSS.setRenderMask(RenderMask::SubsurfaceScattering);

			FXAA& fxaa = FXAA(0.02f, 0.15f);
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);

			
			app.run();
		};


		Configuration config(window, init, TonemappingMethod::ACESFilm);
		config.run();
	}

};

