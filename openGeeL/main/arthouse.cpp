#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "../renderer/shader/shader.h"
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

#include "arthouse.h"

#define pi 3.141592f

using namespace geeL;


SpotLight* spotLight4 = nullptr;

namespace {

	class TestScene4 : public SceneControlObject {

	public:
		LightManager& lightManager;
		MaterialFactory& materialFactory;
		RenderPipeline& shaderManager;
		TransformFactory transformFactory;
		MeshFactory& meshFactory;
		Physics* physics;

		MeshRenderer* nanoRenderer;


		TestScene4(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			RenderPipeline& shaderManager, RenderScene& scene, TransformFactory& transformFactory, Physics* physics)
			: SceneControlObject(scene),
			materialFactory(materialFactory), meshFactory(meshFactory), lightManager(lightManager),
			shaderManager(shaderManager), transformFactory(transformFactory), physics(physics) {}


		virtual void init() {

			float lightIntensity = 10.f;

			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(-0.9f, 1.9f, 0.4f), vec3(-180.0f, 0, -50), vec3(1.f, 1.f, 1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Soft, ShadowmapResolution::Huge);
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config);

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			ImageTexture& texture = materialFactory.CreateTexture("resources/textures/cookie.png",
				ColorType::GammaSpace, WrapMode::Repeat, FilterMode::Linear);

			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-14.88f, 0.4f, -1.88f), vec3(90.f, -56.24f, 179.f), vec3(1.f, 1.f, 1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.00001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			spotLight4 = &lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle, config2);
			spotLight4->setLightCookie(texture);

			float scale = 0.008f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(scale));
			MeshRenderer& studio = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/art/artStudio.obj"),
				meshTransform2, CullingMode::cullFront, "Studio");
			scene.addMeshRenderer(studio);
		}

		virtual void draw(const SceneCamera& camera) {}

		virtual void quit() {}
	};


}



void ArthouseScene::draw() {
	
	RenderWindow window = RenderWindow("geeL", 1920, 1080, WindowMode::Windowed);
	InputManager manager = InputManager();
	manager.defineButton("Forward", GLFW_KEY_W);
	manager.defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(2.93f, 0.71f, -0.59f), vec3(90.f, 76.86f, 179.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 45.f, window.width, window.height, 0.1f, 100.f);

	MaterialFactory materialFactory = MaterialFactory();
	MeshFactory meshFactory = MeshFactory(materialFactory);
	LightManager lightManager = LightManager();
	RenderPipeline shaderManager = RenderPipeline(materialFactory);
	
	RenderScene scene = RenderScene(transFactory.getWorldTransform(), lightManager, shaderManager, camera, materialFactory);
	WorldPhysics physics = WorldPhysics();
	scene.setPhysics(&physics);

	BilateralFilter blur = BilateralFilter(1, 0.7f);
	DefaultPostProcess def = DefaultPostProcess(2.f);
	SSAO ssao = SSAO(blur, 2.f);
	RenderContext context = RenderContext();
	DeferredLighting lighting = DeferredLighting(scene);
	DeferredRenderer& renderer = DeferredRenderer(window, manager, lighting, context, def, materialFactory);
	renderer.addSSAO(ssao, 0.5f);
	renderer.init();

	std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall =
		[&](const Camera& camera, const FrameBuffer& buffer) { renderer.draw(camera, buffer); };

	CubeBuffer cubeBuffer = CubeBuffer();
	BRDFIntegrationMap brdfInt = BRDFIntegrationMap();
	CubeMapFactory cubeMapFactory = CubeMapFactory(cubeBuffer, renderCall, brdfInt);

	Transform& probeTransform = transFactory.CreateTransform(vec3(-6.9f, 1.9f, 2.3f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024, 20, 20, 20);

	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv1/Playa_Sunrise.hdr");
	EnvironmentCubeMap envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 1024);
	IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

	Skybox skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);
	lightManager.addReflectionProbe(probe);
	
	renderer.setScene(scene);
	scene.addRequester(ssao);

	SceneControlObject& testScene = TestScene4(materialFactory, meshFactory, 
		lightManager, shaderManager, scene, transFactory, &physics);

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

	ImageBasedLighting ibl = ImageBasedLighting(scene);
	renderer.addEffect(ibl, ibl);
	
	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR();
	BlurredPostEffect ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.6f, 0.6f);
	
	BloomFilter filter = BloomFilter();
	GaussianBlur& blur5 = GaussianBlur();
	Bloom bloom = Bloom(filter, blur5, 0.4f, 0.2f);

	postLister.add(def);
	postLister.add(ssao);

	renderer.addEffect(ssrrSmooth, ssrr);
	scene.addRequester(ssrr);

	SobelFilter sobel = SobelFilter(15);
	SobelBlur sobelBlur = SobelBlur(sobel);

	GaussianBlur ayy = GaussianBlur();
	VolumetricLight vol = VolumetricLight(*spotLight4, 0.02f, 1.f, 150);
	BlurredPostEffect volSmooth = BlurredPostEffect(vol, ayy, 0.3f, 0.5f);

	VolumetricLightSnippet lightSnippet = VolumetricLightSnippet(vol);
	renderer.addEffect(volSmooth, { &vol, &sobelBlur });
	scene.addRequester(vol);
	postLister.add(volSmooth, lightSnippet);

	FXAA fxaa = FXAA();
	renderer.addEffect(fxaa);

	renderer.linkInformation();
	renderer.render();
}