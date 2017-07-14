#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "../renderer/framebuffer/gbuffer.h"
#include "../renderer/inputmanager.h"
#include "../renderer/window.h"
#include "../renderer/renderer/deferredrenderer.h"
#include "../renderer/scene.h"
#include "../renderer/transformation/transform.h"
#include "../renderer/transformation/transformfactory.h"
#include "../renderer/cameras/perspectivecamera.h"
#include "../renderer/materials/materialfactory.h"
#include "../renderer/lights/lightmanager.h"
#include "../renderer/lighting/raymarcher.h"
#include "../renderer/postprocessing/drawdefault.h"
#include "../renderer/renderer/rendercontext.h"
#include "../renderer/pipeline.h"
#include "raymarchscene.h"

using namespace geeL;

void RaymarchTest::draw() {
	RenderWindow& window = RenderWindow("Raymarch", 1920, 1080, WindowMode::Windowed);
	InputManager manager;

	geeL::Transform& world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory& transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(1.2f, 1.2f, -1.3f), vec3(70.f, 70.f, 180.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 60.f, window.width, window.height, 0.1f, 100.f);

	GBuffer& gBuffer = GBuffer(window.width, window.height);
	MaterialFactory &materialFactory = MaterialFactory(gBuffer);
	LightManager& lightManager = LightManager();
	RenderPipeline& pipeline = RenderPipeline(materialFactory);
	RenderScene& scene = RenderScene(transFactory.getWorldTransform(), lightManager, pipeline, camera, materialFactory);

	DefaultPostProcess& def = DefaultPostProcess(2.f);
	RenderContext& context = RenderContext();
	RayMarcher& raymarch = RayMarcher(scene);
	DeferredRenderer& renderer = DeferredRenderer(window, manager, raymarch, context, def, gBuffer, materialFactory);
	renderer.setScene(scene);

	renderer.render();
}