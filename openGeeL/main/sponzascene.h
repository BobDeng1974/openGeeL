#pragma once

#include "../application/configuration.h"

using namespace geeL;

namespace {

	class MovingLight : public Component {

	public:

		float step = 0.01f;
		virtual void update(Input& input) {
			vec3 position = sceneObject->transform.getPosition();

			if (position.x > 12.5f || position.x < -7.f)
				step = -step;

			if(input.getKey(GLFW_KEY_UP) || input.getKeyHold(GLFW_KEY_UP))
				sceneObject->transform.translate(vec3(0.1f, 0.f, 0.f));
			else if(input.getKey(GLFW_KEY_DOWN) || input.getKeyHold(GLFW_KEY_DOWN))
				sceneObject->transform.translate(vec3(-0.1f, 0.f, 0.f));

			sceneObject->transform.translate(vec3(step, 0.f, 0.f));
		}

	};

}


class SponzaScene {

public:
	static void draw() {
		RenderWindow& window = RenderWindow("Sponza", Resolution(1920, 1080), WindowMode::Windowed);

		
		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory, 
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(-7.36f, 4.76f, -1.75f), vec3(92.6f, -80.2f, 162.8f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(5.f, 0.45f));
			scene.setCamera(camera);
			
			Transform& probeTransform = transformFactory.CreateTransform(vec3(15.15f, 0.62f, -4.11f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
			DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/MonValley_G_DirtRoad_3k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 256);
			//IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(probe);


			float lightIntensity = 3200.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-2.4f, 45.6f, -4.6f), vec3(0.f), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *1.f, lightIntensity * 0.9f, lightIntensity * 0.9f), config);

			lightIntensity = 0.5f;
			Transform& lightTransform3 = transformFactory.CreateTransform(vec3(15.15f, 0.62f, -5.11f), vec3(0.f), vec3(1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.00001f, ShadowMapType::Hard, ShadowmapResolution::Medium);
			lightManager.addPointLight(lightTransform3, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config2);

			Transform& lightTransform4 = transformFactory.CreateTransform(vec3(-8.15f, 0.62f, 4.48f), vec3(0.f), vec3(1.f), true);
			lightManager.addPointLight(lightTransform4, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config2);

			lightIntensity = 55.f;
			Transform& lightTransform5 = transformFactory.CreateTransform(vec3(2.55f, 3.62f, 4.08f), vec3(0.f), vec3(1.f));
			ShadowMapConfiguration config3 = ShadowMapConfiguration(0.001f, ShadowMapType::Soft, ShadowmapResolution::High, 5.f, 10);
			PointLight& point = lightManager.addPointLight(lightTransform5, glm::vec3(lightIntensity *0.148f, lightIntensity *0.0625f, lightIntensity*0.125f), config3);
			point.addComponent<MovingLight>();

			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(4.f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(0.01f));
			MeshRenderer& sponz = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/sponza/sponza.obj"),
				meshTransform6, CullingMode::cullFront, "Sponza");
			scene.addMeshRenderer(sponz);


			ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			gui.addElement(postLister);
			SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
			gui.addElement(sysInfo);


			def.setExposure(3.5f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(1.3f, 0.7f);
			SSAO& ssao = SSAO(blur, 2.5f);
			scene.addRequester(ssao);
			renderer.addEffect(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl, ibl);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.8f, 0.8f);
			renderer.addEffect(ssrrSmooth, ssrr);
			scene.addRequester(ssrr);
			SSRRSnippet& ssrrSnippet = SSRRSnippet(ssrr);
			GaussianBlurSnippet& gaussSnip = GaussianBlurSnippet(blur4);
			postLister.add(ssrrSmooth, ssrrSnippet, gaussSnip);

			BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
			GodRay& ray = GodRay(vec3(-2.4f, 45.6f, -4.6f), 20);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init);
		config.run();
	}


};