#pragma once

#include "../application/configuration.h"

using namespace geeL;


namespace {

	class RotationComponent : public Component {

	public:
		virtual void update(Input& input) {
			sceneObject->transform.rotate(vec3(0.f, 1.f, 0.f), 1.5f * RenderTime::deltaTime());
		}
	};

}

class TestScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("geeL", Resolution(1920, 1080), WindowMode::Windowed);


		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(0.0f, 2.0f, 9.0f), vec3(-90.f, 0.f, 0.f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);
			camera.addComponent<MovableCamera>(MovableCamera(5.f, 0.45f));
			scene.setCamera(camera);

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/Arches_E_PineTree_3k.hdr", 
				cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);


			float lightIntensity = 100.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7, 5, 5), vec3(-180.0f, 0, -50), vec3(1.f));
			lightManager.addPointLight(defPLShadowMapConfig, lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379));

			
			lightIntensity = 15.f;
			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(8.f, 3.5f, 0.f), vec3(145.0f, 50, -160), vec3(1.f));
			ShadowMapConfiguration dlConfig(0.00002f, ShadowMapType::Soft, ShadowmapResolution::Huge, 0.5f, 2U, 15);
			lightManager.addDirectionalLight(dlConfig, lightTransform2, glm::vec3(lightIntensity*0.796, lightIntensity*0.535, lightIntensity*0.379));
			

			float height = -2.f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(20.f, 0.2f, 20.f));
			StaticModelRenderer plane = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/primitives/plane.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform2, false);

			for (auto it(plane.begin()); it != plane.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.setFloatValue("Roughness", 0.25f);
				container.setFloatValue("Metallic", 0.f);
				container.setVectorValue("Color", vec3(0.4f));

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}

			//physics.addPlane(vec3(0.f, 1.f, 0.f), meshTransform2, RigidbodyProperties(0.f, false));


			Transform& meshTransform4 = transformFactory.CreateTransform(vec3(8.f, 2.f, 4.f), vec3(0.f), vec3(1.f));
			StaticModelRenderer sphere = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/primitives/sphere.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform4, false);

			for (auto it(sphere.begin()); it != sphere.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);
				renderer->setShader(ss);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.setVectorValue("Color", vec3(0.6f));
				container.setFloatValue("Transparency", 0.5f);
				container.setFloatValue("Roughness", 0.05f);
				container.setFloatValue("Metallic", 0.5f);

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}

			//physics.addSphere(1.f, meshTransform4, RigidbodyProperties(10.f, false));
			//physics.addMesh(sphere1.getModel(), meshTransform4, RigidbodyProperties(10.f, false));


			Transform& meshTransform42 = transformFactory.CreateTransform(vec3(12.f, 2.f, 4.f), vec3(0.f), vec3(1.f));
			StaticModelRenderer sphere2 = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/primitives/sphere.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform42, false);

			for (auto it(sphere2.begin()); it != sphere2.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				SceneShader& ss = materialFactory.getDefaultShader(ShadingMethod::Transparent, false);
				renderer->setShader(ss);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.setVectorValue("Color", vec3(0.6f));
				container.setFloatValue("Transparency", 0.5f);
				container.setFloatValue("Roughness", 0.05f);
				container.setFloatValue("Metallic", 0.5f);

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}



			Transform& meshTransform5 = transformFactory.CreateTransform(vec3(0.0f, 0.5f, -2.0f), vec3(0.5f, 0.5f, 0.5f), vec3(5.2f, 2.2f, 1.2f));
			StaticModelRenderer box = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/primitives/cube.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform5, false);

			for (auto it(box.begin()); it != box.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.setFloatValue("Roughness", 0.3f);
				container.setFloatValue("Metallic", 0.1f);
				container.setVectorValue("Color", vec3(0.1f, 0.1f, 0.1f));

				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}



			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(4.f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(1.f));
			StaticModelRenderer cyborg = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/cyborg/Cyborg.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform6, false);

			for (auto it(cyborg.begin()); it != cyborg.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);
				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}



			Transform& meshTransform1 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f), vec3(0.2f));
			StaticModelRenderer nano = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/nanosuit/nanosuit.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform1, false);

			MeshRenderer& base = *nano.front();
			base.addComponent<RotationComponent>();

			for (auto it(nano.begin()); it != nano.end(); it++) {
				unique_ptr<StaticMeshRenderer> renderer = std::move(*it);
				scene.addMeshRenderer(std::unique_ptr<MeshRenderer>(std::move(renderer)));
			}


			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(1.f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(1.8f, 7, 0.7f);
			SSAO& ssao = SSAO(blur, 1.f);
			renderer.addEffect(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl, DrawTime::Early);

			BilateralFilter& blur2 = BilateralFilter(1, 7, 0.1f);
			GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 25);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, ResolutionPreset::TWENTY, ResolutionPreset::TWENTY);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, ResolutionPreset::ONETHIRD, ResolutionPreset::ONETHIRD);
			renderer.addEffect(ssrrSmooth);
			scene.addRequester(ssrr);

			MotionBlur& motionBlur = MotionBlur(0.3f, 5.f, 20);
			MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
			renderer.addEffect(motionBlur, DrawTime::Late);
			scene.addRequester(motionBlur);
			postLister.add(mSnippet);

			ColorCorrection& colorCorrect = ColorCorrection();
			renderer.addEffect(colorCorrect, DrawTime::Late);
			postLister.add(colorCorrect);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa, DrawTime::Late);


			app.run();
		};


		Configuration config(window, init, TonemappingMethod::Uncharted2, PhysicsType::World);
		config.run();
	}

};
