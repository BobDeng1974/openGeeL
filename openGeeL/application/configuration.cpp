#include "framebuffer/tbuffer.h"
#include "memory/defaultmemory.h"
#include "memory/simplepool.h"
#include "shader/uniformstack.h"
#include "texturing/textureparams.h"
#include "texturing/textureprovider.h"
#include "shadowmapping/shadowmapadapter.h"
#include "appmanager.h"
#include "configuration.h"

using namespace geeL::memory;

namespace geeL {

	Configuration::Configuration(RenderWindow& window, 
		SceneInitialization initFunction, 
		TonemappingMethod toneMethod,
		ResolutionScale renderScale,
		PhysicsType physicsType)
			: window(window)
			, initFunction(initFunction)
			, toneMethod(toneMethod)
			, physicsType(physicsType)
			, renderScale(renderScale) {}


	void Configuration::run() {
		InputManager manager;
		DefaultMemory memory;
		//SimplePool memory(5000000000);


		geeL::Transform& world = Transform(glm::vec3(0.f), vec3(0.f), vec3(1.f));
		TransformFactory& transFactory = TransformFactory(world);

		Transform& cameraTransform = Transform(vec3(0.f), vec3(0.f), vec3(1.f));
		PerspectiveCamera& defaultCamera = PerspectiveCamera(cameraTransform, 60.f, 
			window.getWidth(), window.getHeight(), 0.1f, 100.f);

		Resolution renderResolution(window.getResolution(), renderScale);
		Texture::setMaxAnisotropyAmount(AnisotropicFilter::Medium);
		TextureProvider textureProvider(renderResolution);

		GBuffer& gBuffer = GBuffer(textureProvider);
		MaterialFactory& materialFactory = MaterialFactory(gBuffer);
		MeshFactory& meshFactory = MeshFactory(materialFactory);
		LightManager& lightManager = LightManager();
		

		UniformBindingStack pipeline;

		RenderScene* s = new RenderScene(transFactory.getWorldTransform(), lightManager, pipeline, 
			defaultCamera, materialFactory, manager);
		RenderScene& scene = *s;

		ShadowmapAdapter shadowAdapter(scene, textureProvider, 4000);
		lightManager.addShadowmapAdapter(shadowAdapter);

		DefaultPostProcess& def = DefaultPostProcess(window.getResolution(), 1.f, toneMethod);
		RenderContext& context = RenderContext();
		SceneRender& lighting = DeferredLighting(scene);
		//SceneRender& lighting = TiledDeferredLighting(scene);
		DeferredRenderer& renderer = DeferredRenderer(window, textureProvider, lighting, context, def, 
			gBuffer, meshFactory, materialFactory);
		renderer.setScene(scene);

		ForwardBuffer& fBuffer = ForwardBuffer(gBuffer, textureProvider);
		renderer.addFBuffer(fBuffer);

		ContinuousSingleThread renderThread(renderer);
		Application& app = ApplicationManager::createApplication(window, manager, renderThread, memory);

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

		app.addThreadedObject(scene);

		initFunction(app, renderer, gui, scene, lightManager, transFactory, meshFactory,
			materialFactory, cubeMapFactory, def, *physics);

		delete physics;
		if (physicsThread != nullptr) delete physicsThread;
		delete s;
	}

}