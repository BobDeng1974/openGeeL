#include "configuration.h"

namespace geeL {

	Configuration::Configuration(RenderWindow& window,
		SceneInitialization initFunction, GBufferContent content, PhysicsType physicsType)
			: window(window), initFunction(initFunction), content(content), physicsType(physicsType) {}


	void Configuration::run() {
		InputManager& manager = InputManager();

		geeL::Transform& world = ThreadedTransform(glm::vec3(0.f), vec3(0.f), vec3(1.f));
		TransformFactory& transFactory = TransformFactory(world);

		Transform& cameraTransform = ThreadedTransform(vec3(0.f), vec3(0.f), vec3(1.f));
		PerspectiveCamera& defaultCamera = PerspectiveCamera(cameraTransform, 0.f, 0.f, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);

		GBuffer& gBuffer = GBuffer(window.resolution, content);
		MaterialFactory& materialFactory = MaterialFactory(gBuffer);
		MeshFactory& meshFactory = MeshFactory(materialFactory);
		LightManager& lightManager = LightManager();
		RenderPipeline& pipeline = RenderPipeline(materialFactory);
		RenderScene& scene = RenderScene(transFactory.getWorldTransform(), lightManager, pipeline, defaultCamera, materialFactory, manager);
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

		Physics* physics;
		ContinuousThread* physicsThread = nullptr;
		if (physicsType == PhysicsType::World) {
			WorldPhysics* wp = new WorldPhysics();
			physicsThread = new ContinuousThread(*wp);
			app.addThread(*physicsThread);

			physics = wp;
		}
		else
			physics = new NoPhysics();

		//ContinuousThread sceneThread(scene);
		//app.addThread(sceneThread);

		initFunction(app, renderer, gui, scene, lightManager, transFactory, meshFactory,
			materialFactory, cubeMapFactory, def, *physics);


		delete physics;
		if (physicsThread != nullptr) delete physicsThread;
	}

}