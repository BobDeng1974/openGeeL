#pragma once

#include "../application/configuration.h"

using namespace geeL;


class SponzaGIScene {


public:
	static void draw() {
		RenderWindow& window = RenderWindow("Global Illumination Sponza", Resolution(1920, 1080), WindowMode::Windowed);

		auto init = [&window](Application& app, PostEffectDrawer& renderer, GUIRenderer& gui, RenderScene& scene,
			LightManager& lightManager, TransformFactory& transformFactory, MeshFactory& meshFactory, MaterialFactory& materialFactory,
			CubeMapFactory& cubeMapFactory, DefaultPostProcess& def, Physics& physics) {


			Transform& cameraTransform = transformFactory.CreateTransform(vec3(41.f, 40.2f, 115.0f), vec3(92.6f, -80.2f, 162.8f), vec3(1.f));
			PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 75.f, window.getWidth(), window.getHeight(), 0.1f, 300.f);
			camera.addComponent<MovableCamera>(MovableCamera(15.f, 0.65f));
			scene.setCamera(camera);

			EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/envmaps/MonValley_G_DirtRoad_3k.hdr");
			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeMapFactory.getBuffer(), 256);
			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);

			float lightIntensity = 500.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(131.f, 72.2f, 128.f), vec3(0.f), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 20.f, 15U, 150.f);
			lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *1.f, lightIntensity * 0.9f, lightIntensity * 0.9f), config);

			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(135.f, 29.f, 121.0f), vec3(0.f, 70.f, 0.f), vec3(15.f));
			std::unique_ptr<MeshRenderer> buddhaPtr = meshFactory.CreateMeshRenderer(
				meshFactory.CreateStaticModel("resources/classics/buddha.obj"),
				meshTransform2, "Buddha");
			MeshRenderer& buddha = scene.addMeshRenderer(std::move(buddhaPtr));

			buddha.iterateMaterials([&](MaterialContainer& container) {
				container.setFloatValue("Transparency", 0.01f);
				container.setFloatValue("Roughness", 0.25f);
				container.setFloatValue("Metallic", 0.4f);
				container.setVectorValue("Color", vec3(0.1f));
			});


			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(152.f, 24.f, 124.0f), vec3(0.f, 0.f, 0.f), vec3(0.08f));
			std::unique_ptr<MeshRenderer> sponzaPtr = meshFactory.CreateMeshRenderer(
				meshFactory.CreateStaticModel("resources/sponza/sponza.obj"),
				meshTransform6, "Sponza");
			MeshRenderer& sponza = scene.addMeshRenderer(std::move(sponzaPtr));

			//DynamicRenderTexture& renderTex = DynamicRenderTexture(camera, Resolution(1000));
			//renderer.addRenderTexture(renderTex);

			sponza.iterateMaterials([&](MaterialContainer& container) {
				if (container.name == "fabric_g") {
					//container.addTexture("Diffuse", renderTex);
					container.setVectorValue("Emissivity", vec3(0.08f));
				}
				else if (container.name == "Material__57")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/sponza/textures/vase_plant_mask.tga"));
				else if (container.name == "chain")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/sponza/textures/chain_texture_mask.tga"));
				else if (container.name == "leaf")
					container.addTexture("alpha", materialFactory.CreateTexture("resources/sponza/textures/sponza_thorn_mask.tga"));

			});


			ObjectLister& objectLister = ObjectLister(scene);
			objectLister.add(camera);
			PostProcessingEffectLister postLister;
			GUILister& lister = GUILister(window, 0.01f, 0.15f, 0.17f, 0.5f, objectLister, postLister);
			gui.addElement(lister);
			gui.addSystemInformation(0.01f, 0.655f, 0.17f, 0.14f);

			def.setExposure(15.f);
			postLister.add(def);

			//Voxelizer& voxelizer = Voxelizer(scene);
			//VoxelOctree& octree = VoxelOctree(voxelizer);
			//VoxelConeTracer& tracer = VoxelConeTracer(scene, octree);

			VoxelTexture& tex = VoxelTexture(scene);
			VoxelConeTracer& tracer = VoxelConeTracer(scene, tex);

			renderer.addEffect(tracer);
			postLister.add(tracer);
			//lightManager.addVoxelStructure(tex);

			/*
			GaussianBlurBase& blur4 = GaussianBlurBase(0.5f);
			MultisampledSSRR& ssrr = MultisampledSSRR(25, 35, 1.f);
			BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.5f, 0.5f);
			renderer.addEffect(ssrrSmooth);
			scene.addRequester(ssrr);
			SSRRSnippet& ssrrSnippet = SSRRSnippet(ssrr);
			GaussianBlurSnippet& gaussSnippet = GaussianBlurSnippet(blur4);
			postLister.add(ssrrSmooth, ssrrSnippet, gaussSnippet);
			*/

			DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.4f);
			DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, 10.f, 35.f,
				camera.getFarPlane(), ResolutionPreset::FULLSCREEN);
			//renderer.addEffect(dof);
			//postLister.add(dof);

			FXAA& fxaa = FXAA();
			renderer.addEffect(fxaa, DrawTime::Late);
			postLister.add(fxaa);


			app.run();
		};


		Configuration config(window, init, GBufferContent::DefaultEmissive);
		config.run();
	}

};
