#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <atomic>
#include "../renderer/framebuffer/gbuffer.h"
#include "../renderer/shader/rendershader.h"
#include "../renderer/renderer/splitrenderer.h"
#include "../renderer/renderer/rendercontext.h"
#include "../application/application.h"
#include "../application/objectwrapper.h"

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
#include "../renderer/postprocessing/motionblur.h"
#include "../renderer/postprocessing/lensflare.h"

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

#include "../renderer/physics/physics.h"
#include "../renderer/physics/worldphysics.h"
#include "../renderer/physics/rigidbody.h"
#include "../renderer/animation/animator.h"
#include "../renderer/animation/skeleton.h"

#include "testscene.h"

using namespace geeL;


namespace {

	class RotationComponent : public Component {

	public:
		virtual void update() {
			sceneObject->transform.rotate(vec3(0.f, 1.f, 0.f), 1.5f * RenderTime::deltaTime);
		}
	};

}


void RenderTest::draw() {
	RenderWindow& window = RenderWindow("geeL", Resolution(1920, 1080), WindowMode::Windowed);
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

	DefaultPostProcess& def = DefaultPostProcess();
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
	renderer.addGUIRenderer(&gui);



	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv2/Arches_E_PineTree_3k.hdr");
	EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 1024);
	IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

	Skybox& skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);
	lightManager.addReflectionProbe(iblMap);
	
	

	float lightIntensity = 100.f;
	Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7, 5, 5), vec3(-180.0f, 0, -50), vec3(1.f));
	&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), defPLShadowMapConfig);

	lightIntensity = 100.f;
	float angle = glm::cos(glm::radians(25.5f));
	float outerAngle = glm::cos(glm::radians(27.5f));

	ImageTexture& texture = materialFactory.CreateTexture("resources/textures/cookie.png",
		ColorType::GammaSpace, WrapMode::Repeat, FilterMode::LinearMip);

	Transform& lightTransform2 = transformFactory.CreateTransform(vec3(0.9f, 5, -22.f), vec3(96.f, -0.1f, -5), vec3(1.f));
	SpotLight& spotLight = lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle, defSLShadowMapConfig);
	spotLight.setLightCookie(texture);

	lightIntensity = 0.5f;
	//geeL::Transform& lightTransform3 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(-120, -30, -180), vec3(1.f));
	//lightManager.addDirectionalLight(scene.getCamera(), lightTransform3, glm::vec3(lightIntensity, lightIntensity, lightIntensity), defDLShadowMapConfig);

	float height = -2.f;
	Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(20.f, 0.2f, 20.f));
	MeshRenderer& plane = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/plane.obj"),
		meshTransform2, CullingMode::cullFront, "Floor");

	scene.addMeshRenderer(plane);
	physics.addPlane(vec3(0.f, 1.f, 0.f), meshTransform2, RigidbodyProperties(0.f, false));

	plane.iterateMaterials([&](MaterialContainer& container) {
		container.setFloatValue("Roughness", 0.25f);
		container.setFloatValue("Metallic", 0.f);
		container.setVectorValue("Color", vec3(0.4f, 0.4f, 0.4f));
	});

	Transform& meshTransform3 = transformFactory.CreateTransform(vec3(-9.f, -3.f, -10.0f), vec3(0.5f, 0.5f, 0.5f), vec3(0.3f));
	MeshRenderer& state = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/empire/EmpireState_lp.obj"),
		meshTransform3, CullingMode::cullFront, "Empire State");
	//scene.addMeshRenderer(state);

	state.iterateMaterials([&](MaterialContainer& container) {
		container.setFloatValue("Roughness", 0.5f);
		container.setVectorValue("Color", vec3(0.5f, 0.5f, 0.5f));
	});


	Transform& meshTransform4 = transformFactory.CreateTransform(vec3(8.f, 2.f, 4.f), vec3(0.f), vec3(1.f));
	MeshRenderer& sphere1 = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/sphere.obj"),
		meshTransform4, CullingMode::cullFront, "Sphere");
	//scene.addMeshRenderer(sphere1);
	//if (physics != nullptr) physics->addSphere(1.f, meshTransform4, RigidbodyProperties(10.f, false));
	//if (physics != nullptr) physics->addMesh(sphere1.getModel(), meshTransform4, RigidbodyProperties(10.f, false));

	sphere1.iterateMaterials([&](MaterialContainer& container) {
		container.setVectorValue("Color", vec3(0.f));
		container.setFloatValue("Roughness", 0.05f);
		container.setFloatValue("Metallic", 0.5f);
	});


	Transform& meshTransform5 = transformFactory.CreateTransform(vec3(0.0f, 0.5f, -2.0f), vec3(0.5f, 0.5f, 0.5f), vec3(5.2f, 2.2f, 1.2f));
	MeshRenderer& box = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/cube.obj"),
		meshTransform5, CullingMode::cullFront, "Box");
	//scene.addMeshRenderer(box);

	box.iterateMaterials([&](MaterialContainer& container) {
		container.setFloatValue("Roughness", 0.3f);
		container.setFloatValue("Metallic", 0.1f);
		container.setVectorValue("Color", vec3(0.1f, 0.1f, 0.1f));
	});


	Transform& meshTransform6 = transformFactory.CreateTransform(vec3(4.f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(1.f));
	MeshRenderer& cyborg = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/cyborg/Cyborg.obj"),
		meshTransform6, CullingMode::cullFront, "Cyborg");
	scene.addMeshRenderer(cyborg);

	Transform& meshTransform1 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(0.2f));
	MeshRenderer& nano = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/nanosuit/nanosuit.obj"),
		meshTransform1, CullingMode::cullFront, "Nano");
	nano.addComponent<RotationComponent>();
	scene.addMeshRenderer(nano);

	/*
	float scale = 0.05f;
	Transform& meshTransform7 = transformFactory.CreateTransform(vec3(2.f, -2.f, 4.0f), vec3(-90.f, 0.f, 0.f), vec3(scale, scale, scale));
	SkinnedMeshRenderer& dude = meshFactory.CreateSkinnedMeshRenderer(meshFactory.CreateSkinnedModel("resources/guard/boblampclean.md5mesh"),
	meshTransform7, CullingMode::cullFront, "Dude");
	scene.AddMeshRenderer(dude);

	SimpleAnimator& anim = dude.addComponent(SimpleAnimator(dude.getSkinnedModel(), dude.getSkeleton()));
	anim.playAnimation(0);
	*/

	

	ObjectLister& objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
	objectLister.add(camera);
	gui.addElement(objectLister);
	PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
	gui.addElement(postLister);
	SystemInformation& sysInfo = SystemInformation(window, 0.01f, 0.74f, 0.17f, 0.075f);
	gui.addElement(sysInfo);
	

	postLister.add(def);

	BilateralFilter& blur = BilateralFilter(1.8f, 0.7f);
	SSAO& ssao = SSAO(blur, 1.f);
	renderer.addSSAO(ssao);
	scene.addRequester(ssao);
	postLister.add(ssao);

	ImageBasedLighting& ibl = ImageBasedLighting(scene);
	renderer.addEffect(ibl, ibl);

	BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
	GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 25);
	BlurredPostEffect& raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);
	GodRaySnippet& godRaySnippet = GodRaySnippet(ray);
	renderer.addEffect(raySmooth);
	scene.addRequester(ray);
	postLister.add(raySmooth, godRaySnippet);

	SobelFilter& sobel = SobelFilter(15);
	SobelBlur& sobelBlur = SobelBlur(sobel);
	VolumetricLight& vol = VolumetricLight(spotLight, 0.1f, 1.f, 160);
	BlurredPostEffect& volSmooth = BlurredPostEffect(vol, sobelBlur, 0.4f, 0.4f);
	//VolumetricLightSnippet lightSnippet = VolumetricLightSnippet(vol);
	//renderer.addEffect(volSmooth, { &vol, &sobelBlur });
	//scene.addRequester(vol);
	//postLister.add(volSmooth, lightSnippet);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR();
	BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.3f, 0.3f);
	renderer.addEffect(ssrrSmooth, ssrr);
	scene.addRequester(ssrr);

	MotionBlur& motionBlur = MotionBlur(0.3f, 20);
	MotionBlurSnippet& mSnippet = MotionBlurSnippet(motionBlur);
	renderer.addEffect(motionBlur);
	scene.addRequester(motionBlur);
	postLister.add(mSnippet);

	ColorCorrection& colorCorrect = ColorCorrection();
	renderer.addEffect(colorCorrect);
	postLister.add(colorCorrect);

	FXAA& fxaa = FXAA();
	renderer.addEffect(fxaa);

	
	app.run();

}