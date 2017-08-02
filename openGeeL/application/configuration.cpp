#include "configuration.h"

namespace geeL {

	Configuration::Configuration(RenderWindow& window, PerspectiveCamera& camera,
		SceneInitialization initFunction, GBufferContent content)
			: window(window), camera(camera), initFunction(initFunction), content(content) {}


	void Configuration::run() {
		InputManager& manager = InputManager();

		geeL::Transform& world = ThreadedTransform(glm::vec3(0.f), vec3(0.f), vec3(1.f));
		TransformFactory& transFactory = TransformFactory(world);

		GBuffer& gBuffer = GBuffer(window.resolution, content);
		MaterialFactory& materialFactory = MaterialFactory(gBuffer);
		MeshFactory& meshFactory = MeshFactory(materialFactory);
		LightManager& lightManager = LightManager();
		RenderPipeline& pipeline = RenderPipeline(materialFactory);
		RenderScene& scene = RenderScene(transFactory.getWorldTransform(), lightManager, pipeline, camera, materialFactory, manager);
		Texture::setMaxAnisotropyAmount(AnisotropicFilter::Medium);


		DefaultPostProcess& def = DefaultPostProcess();
		RenderContext& context = RenderContext();
		DeferredLighting& lighting = DeferredLighting(scene);
		DeferredRenderer& renderer = DeferredRenderer(window, manager, lighting, context, def, gBuffer);
		renderer.setScene(scene);

		Application& app = Application(window, manager, renderer);

		std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall =
			[&](const Camera& camera, const FrameBuffer& buffer) { renderer.draw(camera, buffer); };

		CubeBuffer cubeBuffer;
		BRDFIntegrationMap brdfInt;
		CubeMapFactory& cubeMapFactory = CubeMapFactory(cubeBuffer, renderCall, brdfInt);

		GUIRenderer& gui = GUIRenderer(window, context);
		renderer.addGUIRenderer(&gui);

		Physics& physics = NoPhysics();

		initFunction(app, renderer, gui, scene, lightManager, transFactory, meshFactory,
			materialFactory, cubeMapFactory, def, physics);

	}

}