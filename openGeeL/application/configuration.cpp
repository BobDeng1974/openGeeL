#include "framebuffer/tbuffer.h"
#include "memory/defaultmemory.h"
#include "memory/simplepool.h"
#include "shader/uniformstack.h"
#include "texturing/textureparams.h"
#include "texturing/textureprovider.h"
#include "appmanager.h"
#include "configuration.h"

using namespace geeL::memory;

namespace geeL {

	Configuration::Configuration(RenderWindow& window, 
		SceneInitialization initFunction, 
		GBufferContent content, 
		PhysicsType physicsType)
			: window(window)
			, initFunction(initFunction)
			, content(content)
			, physicsType(physicsType) {}


	void Configuration::run() {
		InputReader inputReader;
		InputManager scriptingInput(inputReader);
		DefaultMemory memory;
		//SimplePool memory(5000000000);


		geeL::Transform& world = Transform(glm::vec3(0.f), vec3(0.f), vec3(1.f));
		TransformFactory& transFactory = TransformFactory(world);

		Transform& cameraTransform = Transform(vec3(0.f), vec3(0.f), vec3(1.f));
		PerspectiveCamera& defaultCamera = PerspectiveCamera(cameraTransform, 60.f, 
			window.getWidth(), window.getHeight(), 0.1f, 100.f);

		GBuffer& gBuffer = GBuffer(window.resolution, content);
		MaterialFactory& materialFactory = MaterialFactory(gBuffer);
		MeshFactory& meshFactory = MeshFactory(materialFactory);
		LightManager& lightManager = LightManager();
		UniformBindingStack pipeline;

		RenderScene* s = new RenderScene(transFactory.getWorldTransform(), lightManager, pipeline, 
			defaultCamera, materialFactory, scriptingInput);
		RenderScene& scene = *s;

		Texture::setMaxAnisotropyAmount(AnisotropicFilter::Medium);
		TextureProvider textureProvider(window, gBuffer);

		DefaultPostProcess& def = DefaultPostProcess();
		RenderContext& context = RenderContext();
		SceneRender& lighting = DeferredLighting(scene);
		//SceneRender& lighting = TiledDeferredLighting(scene);
		DeferredRenderer& renderer = DeferredRenderer(window, textureProvider, lighting, context, def, 
			gBuffer, meshFactory, materialFactory);
		renderer.setScene(scene);

		ForwardBuffer& fBuffer = ForwardBuffer(gBuffer);
		renderer.addFBuffer(fBuffer);

		ContinuousSingleThread renderThread(renderer);
		Application& app = ApplicationManager::createApplication(window, inputReader, renderThread, memory);

		CubeBuffer cubeBuffer;
		BRDFIntegrationMap brdfInt;
		CubeMapFactory& cubeMapFactory = CubeMapFactory(cubeBuffer, renderer, brdfInt);

		GUIRenderer& gui = GUIRenderer(window, context, renderer);
		renderer.addGUIRenderer(gui);


		Physics* physics;
		ContinuousSingleThread* physicsThread = nullptr;
		if (physicsType == PhysicsType::World) {
			WorldPhysics* wp = new WorldPhysics();
			physicsThread = new ContinuousSingleThread(*wp);
			app.addThread(*physicsThread);

			physics = wp;
		}
		else
			physics = new NoPhysics();

		ContinuousSingleThread scriptingThread(scene);
		app.addThread(scriptingThread);

		initFunction(app, renderer, gui, scene, lightManager, transFactory, meshFactory,
			materialFactory, cubeMapFactory, def, *physics);

		delete physics;
		if (physicsThread != nullptr) delete physicsThread;
		delete s;
	}

}