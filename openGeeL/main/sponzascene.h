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

			EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap("resources/envmaps/MonValley_G_DirtRoad_3k.hdr", 
				cubeMapFactory.getBuffer(), 256);
			Skybox& skybox = Skybox(envCubeMap);
			scene.setSkybox(skybox);

			float lightIntensity = 500.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(131.f, 72.2f, 128.f), vec3(0.f), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 5.5f, 3U, 150.f);
			lightManager.addPointLight(config, lightTransform1, glm::vec3(lightIntensity *1.f, lightIntensity * 0.9f, lightIntensity * 0.9f));


			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(135.f, 29.f, 121.0f), vec3(0.f, 70.f, 0.f), vec3(15.f));
			std::list<std::unique_ptr<SingleStaticMeshRenderer>> buddha = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/classics/buddha.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform2, false);

			for (auto it(buddha.begin()); it != buddha.end(); it++) {
				unique_ptr<SingleStaticMeshRenderer> renderer = std::move(*it);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				container.setFloatValue("Transparency", 0.01f);
				container.setFloatValue("Roughness", 0.25f);
				container.setFloatValue("Metallic", 0.4f);
				container.setVectorValue("Color", vec3(0.1f));

				scene.addMeshRenderer(std::unique_ptr<SingleMeshRenderer>(std::move(renderer)));
			}


			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(152.f, 24.f, 124.0f), vec3(0.f, 0.f, 0.f), vec3(0.08f));
			std::list<std::unique_ptr<SingleStaticMeshRenderer>> sponza = meshFactory.createSingleMeshRenderers(
				meshFactory.createStaticModel("resources/sponza/sponza.obj"),
				materialFactory.getDefaultShader(ShadingMethod::Deferred, false),
				meshTransform6, false);

			//MemoryObject<DynamicRenderTexture> renderTex = new DynamicRenderTexture(camera, Resolution(1000));
			//renderer.addRenderTexture(*renderTex);

			for (auto it(sponza.begin()); it != sponza.end(); it++) {
				unique_ptr<SingleStaticMeshRenderer> renderer = std::move(*it);

				MaterialContainer& container = renderer->getMaterial().getMaterialContainer();
				if (container.name == "fabric_g") {
					//container.addTexture("Diffuse", renderTex);
					container.setVectorValue("Emissivity", vec3(0.08f));
				}
				else if (container.name == "Material__57")
					container.addTexture("alpha", materialFactory.createTexture("resources/sponza/textures/vase_plant_mask.tga"));
				else if (container.name == "chain")
					container.addTexture("alpha", materialFactory.createTexture("resources/sponza/textures/chain_texture_mask.tga"));
				else if (container.name == "leaf")
					container.addTexture("alpha", materialFactory.createTexture("resources/sponza/textures/sponza_thorn_mask.tga"));

				scene.addMeshRenderer(std::unique_ptr<SingleMeshRenderer>(std::move(renderer)));
			}


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
			GaussianBlur& blur4 = GaussianBlur(0.5f);
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


		Configuration config(window, init);
		config.run();
	}

};
