#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "../renderer/shader/rendershader.h"
#include "../renderer/renderer/splitrenderer.h"
#include "../renderer/renderer/rendercontext.h"

#include "../renderer/scripting/scenecontrolobject.h"
#include "../renderer/inputmanager.h"
#include "../renderer/window.h"
#include "../renderer/texturing/texture.h"
#include "../renderer/texturing/imagetexture.h"
#include "../renderer/texturing/layeredtexture.h"

#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"
#include "../renderer/lights/light.h"
#include "../renderer/lights/lightmanager.h"
#include "../renderer/lights/pointlight.h"
#include "../renderer/lights/directionallight.h"
#include "../renderer/lights/spotlight.h"
#include "../renderer/renderer/deferredrenderer.h"

#include "../renderer/transformation/transform.h"
#include "../renderer/transformation/transformfactory.h"

#include "../renderer/materials/material.h"
#include "../renderer/materials/materialfactory.h"
#include "../renderer/pipeline.h"
#include "../renderer/meshes/meshrenderer.h"
#include "../renderer/meshes/skinnedrenderer.h"
#include "../renderer/meshes/meshfactory.h"

#include "../renderer/lighting/deferredlighting.h"
#include "../renderer/lighting/ibl.h"
#include "../renderer/postprocessing/postprocessing.h"
#include "../renderer/postprocessing/gaussianblur.h"
#include "../renderer/postprocessing/bloom.h"
#include "../renderer/postprocessing/godray.h"
#include "../renderer/postprocessing/ssao.h"
#include "../renderer/postprocessing/ssrr.h"
#include "../renderer/postprocessing/simpleblur.h"
#include "../renderer/postprocessing/dof.h"
#include "../renderer/postprocessing/fxaa.h"
#include "../renderer/postprocessing/blurredeffect.h"
#include "../renderer/postprocessing/volumetriclight.h"
#include "../renderer/postprocessing/sobel.h"
#include "../renderer/postprocessing/drawdefault.h"

#include "../renderer/cubemapping/cubemap.h"
#include "../renderer/cubemapping/texcubemap.h"
#include "../renderer/texturing/envmap.h"
#include "../renderer/cubemapping/envcubemap.h"
#include "../renderer/cubemapping/irrmap.h"
#include "../renderer/cubemapping/prefilterEnvmap.h"
#include "../renderer/cubemapping/iblmap.h"
#include "../renderer/cubemapping/skybox.h"
#include "../renderer/cubemapping/reflectionprobe.h"
#include "../renderer/texturing/brdfIntMap.h"
#include "../renderer/scene.h"
#include "../renderer/utility/rendertime.h"
#include "../renderer/framebuffer/cubebuffer.h"
#include "../renderer/cubemapping/cubemapfactory.h"

#include "../interface/guirenderer.h"
#include "../interface/elements/objectlister.h"
#include "../interface/snippets/postsnippets.h"
#include "../interface/elements/posteffectlister.h"
#include "../interface/elements/systeminformation.h"

#include "../renderer/physics/physics.h"
#include "../renderer/physics/worldphysics.h"
#include "../renderer/physics/rigidbody.h"
#include "../renderer/animation/animator.h"
#include "../renderer/animation/skeleton.h"

#include "../renderer/lighting/conetracer.h"
#include "../renderer/voxelization/voxelizer.h"
#include "../renderer/voxelization/voxeloctree.h"

#include "giscene.h"

#define pi 3.141592f

using namespace geeL;


namespace {

	class TestScene7 : public SceneControlObject {

	public:
		LightManager& lightManager;
		MaterialFactory& materialFactory;
		RenderPipeline& shaderManager;
		TransformFactory transformFactory;
		MeshFactory& meshFactory;
		Physics* physics;

		PointLight* point;


		TestScene7(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			RenderPipeline& shaderManager, RenderScene& scene, TransformFactory& transformFactory, Physics* physics)
			: SceneControlObject(scene),
			materialFactory(materialFactory), meshFactory(meshFactory), lightManager(lightManager),
			shaderManager(shaderManager), transformFactory(transformFactory), physics(physics) {}


		virtual void init() {
			float lightIntensity = 300.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(144.f, 82.2f, 132.f), vec3(0.f), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Hard, ShadowmapResolution::Huge, 1.f, 8U, 150.f);
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *1.f, lightIntensity * 0.9f, lightIntensity * 0.9f), config);

			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(150.f, 28.f, 124.0f), vec3(0.f, 0.f, 0.f), vec3(0.08f));
			MeshRenderer& sponz = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/sponza/sponza.obj"),
				meshTransform6, CullingMode::cullFront, "Sponza");
			scene.addMeshRenderer(sponz);
		}

		virtual void draw(const SceneCamera& camera) {}
		virtual void quit() {}
	};
}


void VoxelScene::draw() {

	RenderWindow window = RenderWindow("geeL", 1920, 1080, WindowMode::Windowed);
	InputManager manager = InputManager();
	manager.defineButton("Forward", GLFW_KEY_W);
	manager.defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(41.f, 40.2f, 115.0f), vec3(92.6f, -80.2f, 162.8f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera = PerspectiveCamera(cameraTransform, 15.f, 0.45f, 60.f, window.width, window.height, 0.1f, 500.f);

	MaterialFactory materialFactory = MaterialFactory();
	MeshFactory meshFactory = MeshFactory(materialFactory);
	LightManager lightManager = LightManager();
	RenderPipeline shaderManager = RenderPipeline(materialFactory);
	RenderScene scene = RenderScene(transFactory.getWorldTransform(), lightManager, shaderManager, camera, materialFactory);
	Texture::setMaxAnisotropyAmount(AnisotropicFilter::Medium);

	BilateralFilter blur = BilateralFilter(1, 0.7f);
	DefaultPostProcess def = DefaultPostProcess(3.5f);
	SSAO ssao = SSAO(blur, 2.5f);
	RenderContext context = RenderContext();
	DeferredLighting lighting = DeferredLighting(scene);
	DeferredRenderer& renderer = DeferredRenderer(window, manager, lighting, context, def, materialFactory);
	//renderer.addSSAO(ssao, 0.5f);
	renderer.init();

	std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall =
		[&](const Camera& camera, const FrameBuffer& buffer) { renderer.draw(camera, buffer); };

	CubeBuffer cubeBuffer = CubeBuffer();
	BRDFIntegrationMap brdfInt = BRDFIntegrationMap();
	CubeMapFactory cubeMapFactory = CubeMapFactory(cubeBuffer, renderCall, brdfInt);

	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv4/MonValley_G_DirtRoad_3k.hdr");
	EnvironmentCubeMap envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 256);
	IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

	Skybox skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);
	lightManager.addReflectionProbe(iblMap);

	renderer.setScene(scene);
	scene.addRequester(ssao);

	SceneControlObject& testScene = TestScene7(materialFactory, meshFactory,
		lightManager, shaderManager, scene, transFactory, nullptr);

	renderer.addObject(&testScene);
	renderer.initObjects();

	GUIRenderer gui = GUIRenderer(window, context);
	ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
	objectLister.add(camera);
	gui.addElement(objectLister);
	PostProcessingEffectLister postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
	gui.addElement(postLister);
	SystemInformation sysInfo = SystemInformation(renderer.getRenderTime(), window, 0.01f, 0.74f, 0.17f);
	gui.addElement(sysInfo);
	renderer.addGUIRenderer(&gui);

	postLister.add(def);
	postLister.add(ssao);

	Voxelizer voxelizer = Voxelizer(scene);
	VoxelOctree octree = VoxelOctree(voxelizer);

	VoxelConeTracer tracer = VoxelConeTracer(scene, octree, voxelizer);
	renderer.addEffect(tracer, tracer);

	BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
	GodRay& ray = GodRay(vec3(-2.4f, 45.6f, -4.6f), 20);
	BlurredPostEffect raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);
	GodRaySnippet godRaySnippet = GodRaySnippet(ray);
	//renderer.addEffect(raySmooth);
	//scene.addRequester(ray);
	//postLister.add(raySmooth, godRaySnippet);

	FXAA fxaa = FXAA();
	renderer.addEffect(fxaa);
	postLister.add(fxaa);

	renderer.linkInformation();
	renderer.render();
}