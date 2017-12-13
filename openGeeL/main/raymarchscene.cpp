#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "framebuffer/gbuffer.h"
#include "inputmanager.h"
#include "window.h"
#include "renderer/deferredrenderer.h"
#include "renderscene.h"
#include "transformation/transform.h"
#include "transformation/transformfactory.h"
#include "cameras/perspectivecamera.h"
#include "cameras/movablecamera.h"
#include "materials/materialfactory.h"
#include "lights/lightmanager.h"
#include "lighting/raymarcher.h"
#include "postprocessing/drawdefault.h"
#include "meshes/meshfactory.h"
#include "renderer/rendercontext.h"
#include "texturing/textureprovider.h"
#include "shader/uniformstack.h"
#include "memory/defaultmemory.h"
#include "application.h"
#include "appmanager.h"
#include "raymarchscene.h"

using namespace geeL;

void RaymarchTest::draw() {
	RenderWindow& window = RenderWindow("Raymarch", Resolution(1920, 1080), WindowMode::Windowed);
	InputManager manager;
	memory::DefaultMemory* memory = new memory::DefaultMemory();

	geeL::Transform& world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory& transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(1.2f, 1.2f, -1.3f), vec3(70.f, 70.f, 180.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);

	GBuffer& gBuffer = GBuffer(window.resolution);
	TextureProvider textureProvider(window, gBuffer);
	MaterialFactory &materialFactory = MaterialFactory(gBuffer);
	MeshFactory& meshFactory = MeshFactory(materialFactory);
	LightManager& lightManager = LightManager();
	UniformBindingStack pipeline;
	RenderScene& scene = RenderScene(transFactory.getWorldTransform(), lightManager, pipeline, camera, materialFactory, manager);

	DefaultPostProcess& def = DefaultPostProcess(2.f);
	RenderContext& context = RenderContext();
	RayMarcher& raymarch = RayMarcher(scene);
	DeferredRenderer& renderer = DeferredRenderer(window, textureProvider, raymarch, context, def, 
		gBuffer, meshFactory, materialFactory);
	renderer.setScene(scene);

	ContinuousSingleThread renderThread(renderer);
	Application& app = ApplicationManager::createApplication(window, manager, renderThread, *memory);

	ContinuousSingleThread scriptingThread(scene);
	app.addThread(scriptingThread);

	camera.addComponent<MovableCamera>(MovableCamera(5.f, 0.45f));

	app.run();
}