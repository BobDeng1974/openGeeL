#pragma once

#include "../application/configuration.h"

using namespace geeL;


namespace {

	class RotationComponent : public Component {

	public:
		virtual void update(Input& input) {
			sceneObject->transform.rotate(vec3(0.f, 1.f, 0.f), 1.5f * RenderTime::deltaTime);
		}
	};

}

class TestScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("geeL", Resolution(1920, 1080), WindowMode::Windowed);

		Transform& cameraTransform = Transform(vec3(0.0f, 2.0f, 9.0f), vec3(-90.f, 0.f, 0.f), vec3(1.f));
		PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);

		auto init = [&window, &camera](Application& app, DeferredRenderer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv2/Arches_E_PineTree_3k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 1024);
			IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);
			lightManager.addReflectionProbe(iblMap);



			float lightIntensity = 100.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7, 5, 5), vec3(-180.0f, 0, -50), vec3(1.f));
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), defPLShadowMapConfig);

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			ImageTexture& texture = materialFactory.CreateTexture("resources/textures/cookie.png",
				ColorType::GammaSpace, WrapMode::Repeat, FilterMode::LinearMip);

			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(0.9f, 5, -22.f), vec3(96.f, -0.1f, -5), vec3(1.f));
			SpotLight& spotLight = lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle, defSLShadowMapConfig);
			spotLight.setLightCookie(texture);

			lightIntensity = 0.5f;
			//geeL::Transform& lightTransform3 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(-120, -30, -180), vec3(1.f));
			//lightManager.addDirectionalLight(scene.getCamera(), lightTransform3, glm::vec3(lightIntensity, lightIntensity, lightIntensity), defDLShadowMapConfig);

			float height = -2.f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(20.f, 0.2f, 20.f));
			MeshRenderer& plane = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/plane.obj"),
				meshTransform2, CullingMode::cullFront, "Floor");

			scene.addMeshRenderer(plane);
			//physics.addPlane(vec3(0.f, 1.f, 0.f), meshTransform2, RigidbodyProperties(0.f, false));

			plane.iterateMaterials([&](MaterialContainer& container) {
				container.setFloatValue("Roughness", 0.25f);
				container.setFloatValue("Metallic", 0.f);
				container.setVectorValue("Color", vec3(0.4f, 0.4f, 0.4f));
			});

			Transform& meshTransform3 = transformFactory.CreateTransform(vec3(-9.f, -3.f, -10.0f), vec3(0.5f, 0.5f, 0.5f), vec3(0.3f));
			MeshRenderer& state = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/empire/EmpireState_lp.obj"),
				meshTransform3, CullingMode::cullFront, "Empire State");
			//scene.addMeshRenderer(state);

			state.iterateMaterials([&](MaterialContainer& container) {
				container.setFloatValue("Roughness", 0.5f);
				container.setVectorValue("Color", vec3(0.5f, 0.5f, 0.5f));
			});


			Transform& meshTransform4 = transformFactory.CreateTransform(vec3(8.f, 2.f, 4.f), vec3(0.f), vec3(1.f));
			MeshRenderer& sphere1 = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/sphere.obj"),
				meshTransform4, CullingMode::cullFront, "Sphere");
			//scene.addMeshRenderer(sphere1);
			//physics.addSphere(1.f, meshTransform4, RigidbodyProperties(10.f, false));
			//physics.addMesh(sphere1.getModel(), meshTransform4, RigidbodyProperties(10.f, false));

			sphere1.iterateMaterials([&](MaterialContainer& container) {
				container.setVectorValue("Color", vec3(0.f));
				container.setFloatValue("Roughness", 0.05f);
				container.setFloatValue("Metallic", 0.5f);
			});


			Transform& meshTransform5 = transformFactory.CreateTransform(vec3(0.0f, 0.5f, -2.0f), vec3(0.5f, 0.5f, 0.5f), vec3(5.2f, 2.2f, 1.2f));
			MeshRenderer& box = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/cube.obj"),
				meshTransform5, CullingMode::cullFront, "Box");
			//scene.addMeshRenderer(box);

			box.iterateMaterials([&](MaterialContainer& container) {
				container.setFloatValue("Roughness", 0.3f);
				container.setFloatValue("Metallic", 0.1f);
				container.setVectorValue("Color", vec3(0.1f, 0.1f, 0.1f));
			});


			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(4.f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(1.f));
			MeshRenderer& cyborg = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/cyborg/Cyborg.obj"),
				meshTransform6, CullingMode::cullFront, "Cyborg");
			scene.addMeshRenderer(cyborg);

			Transform& meshTransform1 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(0.2f));
			MeshRenderer& nano = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/nanosuit/nanosuit.obj"),
				meshTransform1, CullingMode::cullFront, "Nano");
			nano.addComponent<RotationComponent>();
			scene.addMeshRenderer(nano);

			/*
			float scale = 0.05f;
			Transform& meshTransform7 = transformFactory.CreateTransform(vec3(2.f, -2.f, 4.0f), vec3(-90.f, 0.f, 0.f), vec3(scale, scale, scale));
			SkinnedMeshRenderer& dude = meshFactory.CreateSkinnedMeshRenderer(meshFactory.CreateSkinnedModel("resources/guard/boblampclean.md5mesh"),
			meshTransform7, CullingMode::cullFront, "Dude");
			scene.AddMeshRenderer(dude);

			SimpleAnimator& anim = dude.addComponent(SimpleAnimator(dude.getSkinnedModel(), dude.getSkeleton()));
			anim.playAnimation(0);
			*/


			ObjectLister& objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
			objectLister.add(camera);
			gui.addElement(objectLister);
			PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
			gui.addElement(postLister);
			SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
			gui.addElement(sysInfo);


			def.setExposure(1.f);
			postLister.add(def);

			BilateralFilter& blur = BilateralFilter(1.8f, 0.7f);
			SSAO& ssao = SSAO(blur, 1.f);
			renderer.addSSAO(ssao);
			scene.addRequester(ssao);
			postLister.add(ssao);

			ImageBasedLighting& ibl = ImageBasedLighting(scene);
			renderer.addEffect(ibl, ibl);

			BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
			GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 25);
			BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);
			GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
			renderer.addEffect(raySmooth);
			scene.addRequester(ray);
			postLister.add(raySmooth, godRaySnippet);

			SobelFilter& sobel = SobelFilter(15);
			SobelBlur& sobelBlur = SobelBlur(sobel);
			VolumetricLight& vol = VolumetricLight(spotLight, 0.1f, 1.f, 160);
			BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, 0.4f, 0.4f);
			//VolumetricLightSnippet lightSnippet = VolumetricLightSnippet(vol);
			//renderer.addEffect(volSmooth, { &vol, &sobelBlur });
			//scene.addRequester(vol);
			//postLister.add(volSmooth, lightSnippet);

			GaussianBlur& blur4 = GaussianBlur();
			SSRR& ssrr = SSRR();
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.3f, 0.3f);
			renderer.addEffect(ssrrSmooth, ssrr);
			scene.addRequester(ssrr);

			MotionBlur& motionBlur = MotionBlur(0.3f, 20);
			MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
			renderer.addEffect(motionBlur);
			scene.addRequester(motionBlur);
			postLister.add(mSnippet);

			ColorCorrection& colorCorrect = ColorCorrection();
			renderer.addEffect(colorCorrect);
			postLister.add(colorCorrect);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa);


			app.run();
		};


		Configuration config(window, camera, init, GBufferContent::Default, PhysicsType::World);
		config.run();
	}

};
