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

#include "bedroomscene.h"

#define pi 3.141592f

using namespace geeL;


SpotLight* spotLight3 = nullptr;

namespace {

	class TestScene3 : public SceneControlObject {

	public:
		LightManager& lightManager;
		MaterialFactory& materialFactory;
		RenderPipeline& shaderManager;
		TransformFactory transformFactory;
		MeshFactory& meshFactory;
		Physics* physics;


		TestScene3(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			RenderPipeline& shaderManager, RenderScene& scene, TransformFactory& transformFactory, Physics* physics)
			: SceneControlObject(scene),
			materialFactory(materialFactory), meshFactory(meshFactory), lightManager(lightManager),
			shaderManager(shaderManager), transformFactory(transformFactory), physics(physics) {}


		virtual void init() {

			float lightIntensity = 50.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(0.01f, 9.4f, -0.1f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.0001f, ShadowMapType::Soft, ShadowmapResolution::Huge, 14.f, 14);
			lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity * 0.69, lightIntensity * 0.32, lightIntensity * 0.22), config);

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-23.4f, 19.69f, -8.7f), vec3(123.4f, 58.5f, 2.9f), vec3(1.f), true);
			ShadowMapConfiguration config2 = ShadowMapConfiguration(0.0001f, ShadowMapType::Hard, ShadowmapResolution::Huge);
			spotLight3 = &lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity * 0.85f, lightIntensity * 0.87f, lightIntensity * 0.66f), angle, outerAngle, config2);

			float scale = 0.05f;
			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(scale));
			MeshRenderer& bedroom = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/bedroom/Bedroom2.obj"),
				meshTransform2, CullingMode::cullFront, "Bedroom");
			scene.addMeshRenderer(bedroom);
		}

		virtual void draw(const SceneCamera& camera) {}

		virtual void quit() {}
	};


}



void BedroomScene::draw() {
	RenderWindow& window = RenderWindow("Bedroom", 1920, 1080, WindowMode::Windowed);
	InputManager manager;

	geeL::Transform& world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory& transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(5.4f, 10.0f, -2.9f), vec3(70.f, 50.f, -175.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera& camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 60.f, window.width, window.height, 0.1f, 100.f);

	GBuffer& gBuffer = GBuffer(window.width, window.height);
	MaterialFactory& materialFactory = MaterialFactory(gBuffer);
	MeshFactory& meshFactory = MeshFactory(materialFactory);
	LightManager lightManager;
	RenderPipeline& shaderManager = RenderPipeline(materialFactory);
	
	RenderScene& scene = RenderScene(transFactory.getWorldTransform(), lightManager, shaderManager, camera, materialFactory);
	WorldPhysics& physics = WorldPhysics();
	scene.setPhysics(&physics);

	BilateralFilter& blur = BilateralFilter(1, 0.7f);
	DefaultPostProcess& def = DefaultPostProcess(9.f);
	SSAO& ssao = SSAO(blur, 10.f);
	RenderContext context;
	DeferredLighting& lighting = DeferredLighting(scene);
	DeferredRenderer& renderer = DeferredRenderer(window, manager, lighting, context, def, gBuffer, materialFactory);
	renderer.addSSAO(ssao, 0.5f);
	renderer.init();

	std::function<void(const Camera&, const FrameBuffer& buffer)> renderCall =
		[&](const Camera& camera, const FrameBuffer& buffer) { renderer.draw(camera, buffer); };

	CubeBuffer cubeBuffer;
	BRDFIntegrationMap brdfInt;
	CubeMapFactory& cubeMapFactory = CubeMapFactory(cubeBuffer, renderCall, brdfInt);

	Transform& probeTransform = transFactory.CreateTransform(vec3(0.5f, 7.1f, 5.5f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	DynamicIBLMap& probe = cubeMapFactory.createReflectionProbeIBL(probeTransform, 1024, 20, 20, 20);

	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv3/Tropical_Beach_3k.hdr");
	EnvironmentCubeMap& envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 1024);
	IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

	Skybox& skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);
	lightManager.addReflectionProbe(probe);
	
	renderer.setScene(scene);
	scene.addRequester(ssao);

	SceneControlObject& testScene = TestScene3(materialFactory, meshFactory, 
		lightManager, shaderManager, scene, transFactory, &physics);

	renderer.addObject(&testScene);
	renderer.initObjects();

	GUIRenderer& gui = GUIRenderer(window, context);
	ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.35f);
	objectLister.add(camera);
	gui.addElement(objectLister);
	PostProcessingEffectLister& postLister = PostProcessingEffectLister(window, 0.01f, 0.375f, 0.17f, 0.35f);
	gui.addElement(postLister);
	SystemInformation& sysInfo = SystemInformation(renderer.getRenderTime(), window, 0.01f, 0.74f, 0.17f);
	gui.addElement(sysInfo);
	renderer.addGUIRenderer(&gui);

	ImageBasedLighting& ibl = ImageBasedLighting(scene);
	renderer.addEffect(ibl, ibl);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR();
	//MultisampledSSRR& ssrr = MultisampledSSRR();
	BlurredPostEffect& ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.5f, 0.5f);
	
	DepthOfFieldBlur& blur3 = DepthOfFieldBlur(2, 0.3f);
	DepthOfFieldBlurred& dof = DepthOfFieldBlurred(blur3, camera.depth, 4.f, 100.f, 0.3f);

	GaussianBlur& ayy = GaussianBlur();
	VolumetricLight& vol = VolumetricLight(*spotLight3, 0.7f, 14.f, 250);
	BlurredPostEffect& volSmooth = BlurredPostEffect(vol, ayy, 0.25f, 0.2f);

	postLister.add(def);
	postLister.add(ssao);

	VolumetricLightSnippet& lightSnippet = VolumetricLightSnippet(vol);
	renderer.addEffect(volSmooth, { &vol});
	scene.addRequester(vol);
	postLister.add(volSmooth, lightSnippet);

	renderer.addEffect(ssrrSmooth, ssrr);
	scene.addRequester(ssrr);
	SSRRSnippet& ssrrSnippet = SSRRSnippet(ssrr);
	postLister.add(ssrrSmooth, ssrrSnippet);

	renderer.addEffect(dof, dof);
	postLister.add(dof);

	FXAA& fxaa = FXAA(0.f, 0.f);
	renderer.addEffect(fxaa);
	postLister.add(fxaa);

	renderer.linkInformation();
	renderer.render();
}