#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "../renderer/framebuffer/gbuffer.h"
#include "../renderer/shader/rendershader.h"
#include "../renderer/renderer/splitrenderer.h"
#include "../renderer/renderer/rendercontext.h"
#include "../application/application.h"

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
#include "../interface/snippets/blursnippets.h"
#include "../interface/elements/posteffectlister.h"
#include "../interface/elements/systeminformation.h"

#include "../physics/physics.h"
#include "../physics/worldphysics.h"
#include "../physics/rigidbody.h"
#include "../renderer/animation/animator.h"
#include "../renderer/animation/skeleton.h"

#include "../renderer/lighting/conetracer.h"
#include "../renderer/voxelization/voxelizer.h"
#include "../renderer/voxelization/voxeloctree.h"
#include "../renderer/voxelization/voxeltexture.h"

#include "giscene.h"

using namespace geeL;


void SponzaGIScene::draw() {
	RenderWindow& window = RenderWindow("Global Illumination Sponza", Resolution(1920, 1080), WindowMode::Windowed);
	InputManager manager;

	geeL::Transform& world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory& transformFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(41.f, 40.2f, 115.0f), vec3(92.6f, -80.2f, 162.8f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 15.f, 0.65f, 60.f, window.getWidth(), window.getHeight(), 0.1f, 500.f);

	GBuffer& gBuffer = GBuffer(window.resolution, GBufferContent::DefaultEmissive);
	MaterialFactory& materialFactory = MaterialFactory(gBuffer);
	MeshFactory& meshFactory = MeshFactory(materialFactory);
	LightManager lightManager;
	RenderPipeline& shaderManager = RenderPipeline(materialFactory);
	RenderScene& scene = RenderScene(transformFactory.getWorldTransform(), lightManager, shaderManager, camera, materialFactory, manager);
	Texture::setMaxAnisotropyAmount(AnisotropicFilter::Medium);

	DefaultPostProcess& def = DefaultPostProcess(15.f);
	RenderContext context;
	DeferredLighting& lighting = DeferredLighting(scene);
	DeferredRenderer& renderer = DeferredRenderer(window, manager, lighting, context, def, gBuffer);
	renderer.setScene(scene);

	Application& app = Application(window, manager, renderer);

	std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall =
		[&](const Camera& camera, const FrameBuffer& buffer) { renderer.draw(camera, buffer); };

	CubeBuffer cubeBuffer;
	
	GUIRenderer& gui = GUIRenderer(window, context);
	renderer.addGUIRenderer(&gui);


	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv4/MonValley_G_DirtRoad_3k.hdr");
	EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 256);
	Skybox& skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);



	float lightIntensity = 500.f;
	Transform& lightTransform1 = transformFactory.CreateTransform(vec3(131.f, 72.2f, 128.f), vec3(0.f), vec3(1.f), true);
	ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 20.f, 15U, 150.f);
	lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *1.f, lightIntensity * 0.9f, lightIntensity * 0.9f), config);


	Transform& meshTransform2 = transformFactory.CreateTransform(vec3(135.f, 29.f, 121.0f), vec3(0.f, 70.f, 0.f), vec3(15.f));
	MeshRenderer& buddha = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/classics/buddha.obj"),
		meshTransform2, CullingMode::cullFront, "Buddha");
	scene.addMeshRenderer(buddha);

	buddha.iterateMaterials([&](MaterialContainer& container) {
		container.setFloatValue("Transparency", 0.01f);
		container.setFloatValue("Roughness", 0.25f);
		container.setFloatValue("Metallic", 0.4f);
		container.setVectorValue("Color", vec3(0.1f));
	});


	Transform& meshTransform6 = transformFactory.CreateTransform(vec3(152.f, 24.f, 124.0f), vec3(0.f, 0.f, 0.f), vec3(0.08f));
	MeshRenderer& sponz = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/sponza/sponza.obj"),
		meshTransform6, CullingMode::cullFront, "Sponza");
	scene.addMeshRenderer(sponz);

	sponz.iterateMaterials([&](MaterialContainer& container) {
		if (container.name == "fabric_g")
			container.setVectorValue("Emissivity", vec3(0.08f));
	});
	


	ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
	objectLister.add(camera);
	gui.addElement(objectLister);
	PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
	gui.addElement(postLister);
	SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
	gui.addElement(sysInfo);
	

	postLister.add(def);

	//Voxelizer& voxelizer = Voxelizer(scene);
	//VoxelOctree& octree = VoxelOctree(voxelizer);
	//VoxelConeTracer& tracer = VoxelConeTracer(scene, octree);

	VoxelTexture& tex = VoxelTexture(scene);
	VoxelConeTracer& tracer = VoxelConeTracer(scene, tex);

	renderer.addEffect(tracer, tracer);
	postLister.add(tracer);
	//lightManager.addVoxelStructure(tex);

	/*
	GaussianBlurBase& blur4 = GaussianBlurBase(0.5f);
	MultisampledSSRR& ssrr = MultisampledSSRR(25, 35, 1.f);
	BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.5f, 0.5f);
	renderer.addEffect(ssrrSmooth, ssrr);
	scene.addRequester(ssrr);
	SSRRSnippet& ssrrSnippet = SSRRSnippet(ssrr);
	GaussianBlurSnippet& gaussSnippet = GaussianBlurSnippet(blur4);
	postLister.add(ssrrSmooth, ssrrSnippet, gaussSnippet);
	*/

	DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.4f);
	DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, camera.depth, 35.f, camera.getFarPlane(), 1.f);
	//renderer.addEffect(dof, dof);
	//postLister.add(dof);

	FXAA& fxaa = FXAA();
	renderer.addEffect(fxaa);
	postLister.add(fxaa);

	
	app.run();
}