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
#include "../renderer/materials/defaultmaterial.h"
#include "../renderer/materials/genericmaterial.h"
#include "../renderer/materials/materialfactory.h"
#include "../renderer/pipeline.h"
#include "../renderer/meshes/mesh.h"
#include "../renderer/meshes/model.h"
#include "../renderer/meshes/meshrenderer.h"
#include "../renderer/meshes/skinnedrenderer.h"
#include "../renderer/meshes/meshfactory.h"

#include "../renderer/lighting/ibl.h"
#include "../renderer/lighting/deferredlighting.h"
#include "../renderer/postprocessing/postprocessing.h"
#include "../renderer/postprocessing/colorcorrection.h"
#include "../renderer/postprocessing/gammacorrection.h"
#include "../renderer/postprocessing/tonemapping.h"
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
#include "renderscene.h"
#include "../renderer/utility/rendertime.h"
#include "../renderer/framebuffer/cubebuffer.h"
#include "../renderer/cubemapping/cubemapfactory.h"

#include "../interface/guirenderer.h"
#include "../interface/elements/objectlister.h"
#include "../interface/snippets/postsnippets.h"
#include "../interface/elements/posteffectlister.h"
#include "../interface/elements/systeminformation.h"

#include "../physics/physics.h"
#include "../physics/worldphysics.h"
#include "../physics/rigidbody.h"
#include "../renderer/animation/animator.h"
#include "../renderer/animation/skeleton.h"

#include "sciencescene.h"

using namespace geeL;


void ScieneScene::draw() {
	RenderWindow& window = RenderWindow("Science Scene", Resolution(1920, 1080), WindowMode::Windowed);
	InputManager manager;

	geeL::Transform& world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory& transformFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(0.0f, 2.0f, 9.0f), vec3(-90.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 60.f, window.getWidth(), window.getHeight(), 0.1f, 100.f);

	GBuffer& gBuffer = GBuffer(window.resolution);
	MaterialFactory& materialFactory = MaterialFactory(gBuffer);
	MeshFactory& meshFactory = MeshFactory(materialFactory);
	LightManager lightManager;
	RenderPipeline& shaderManager = RenderPipeline(materialFactory);
	
	RenderScene& scene = RenderScene(transformFactory.getWorldTransform(), lightManager, shaderManager, camera, materialFactory, manager);
	WorldPhysics& physics = WorldPhysics();

	DefaultPostProcess& def = DefaultPostProcess(2.f);
	RenderContext context;
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
	//renderer.addGUIRenderer(&gui);



	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv1/Playa_Sunrise.hdr");
	EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 1024);
	IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

	Skybox& skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);
	lightManager.addReflectionProbe(iblMap);
	

	float lightIntensity = 100.f;
	Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7.1f, 4.9f, 2.4f), vec3(-180.0f, 0, -50), vec3(1.f, 1.f, 1.f), true);
	&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), defPLShadowMapConfig);

	lightIntensity = 100.f;
	float angle = glm::cos(glm::radians(25.5f));
	float outerAngle = glm::cos(glm::radians(27.5f));

	ShadowMapConfiguration config = ShadowMapConfiguration(0.0001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
	Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-11, 11, -15), vec3(118.0f, 40, -23), vec3(1.f, 1.f, 1.f), true);
	SpotLight& spotLight = lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle, config);


	Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(0.1f, 0.1f, 0.1f));
	MeshRenderer& science = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/mad/madScience.obj"),
		meshTransform2, CullingMode::cullFront, "Science");
	scene.addMeshRenderer(science);
	

	ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
	objectLister.add(camera);
	gui.addElement(objectLister);
	PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
	gui.addElement(postLister);
	SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
	gui.addElement(sysInfo);


	postLister.add(def);

	BilateralFilter& blur = BilateralFilter(1, 0.7f);
	SSAO& ssao = SSAO(blur, 3.f);
	renderer.addSSAO(ssao);
	scene.addRequester(ssao);
	postLister.add(ssao);

	ImageBasedLighting& ibl = ImageBasedLighting(scene);
	renderer.addEffect(ibl, ibl);

	SobelFilter& sobel = SobelFilter(15);
	SobelBlur& sobelBlur = SobelBlur(sobel);
	VolumetricLight& vol = VolumetricLight(spotLight, 0.05f, 6.f, 100);
	BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, 0.3f, 0.3f);
	VolumetricLightSnippet& lightSnippet = VolumetricLightSnippet(vol);
	renderer.addEffect(volSmooth, { &vol, &sobelBlur });
	scene.addRequester(vol);
	postLister.add(volSmooth, lightSnippet);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR();
	BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.6f, 0.6f);
	renderer.addEffect(ssrrSmooth, ssrr);
	scene.addRequester(ssrr);

	DepthOfFieldBlur& blur3 = DepthOfFieldBlur(0.3f);
	DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, camera.depth, 5.f, 100.f, 0.3f);
	renderer.addEffect(dof, dof);
	postLister.add(dof);

	FXAA& fxaa = FXAA();
	renderer.addEffect(fxaa);

	
	app.run();
}