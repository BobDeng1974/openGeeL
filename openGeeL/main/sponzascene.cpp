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
#include "../renderer/lighting/light.h"
#include "../renderer/lighting/lightmanager.h"
#include "../renderer/lighting/pointlight.h"
#include "../renderer/lighting/directionallight.h"
#include "../renderer/lighting/spotlight.h"
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

#include "../renderer/postprocessing/deferredlighting.h"
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

#include "sponzascene.h"

#define pi 3.141592f

using namespace geeL;


SpotLight* spotLight6 = nullptr;

namespace {

	class TestScene6 : public SceneControlObject {

	public:
		LightManager& lightManager;
		MaterialFactory& materialFactory;
		RenderPipeline& shaderManager;
		TransformFactory transformFactory;
		MeshFactory& meshFactory;
		Physics* physics;

		MeshRenderer* nanoRenderer;


		TestScene6(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			RenderPipeline& shaderManager, RenderScene& scene, TransformFactory& transformFactory, Physics* physics)
				: SceneControlObject(scene),
					materialFactory(materialFactory), meshFactory(meshFactory), lightManager(lightManager),
					shaderManager(shaderManager), transformFactory(transformFactory), physics(physics) {}


		virtual void init() {

			float lightIntensity = 100.f;
			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(11.7f, 0.6f, -1.1f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			ShadowMapConfiguration config = ShadowMapConfiguration(0.00001f, ShadowMapType::Hard, ShadowmapResolution::Huge, 100.f);
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.15f , lightIntensity *0.14f, lightIntensity*0.12f), config);

			Transform& lightTransform3 = transformFactory.CreateTransform(vec3(-0.3f, 5.f, -0.5f), vec3(-180.0f, 0, -50), vec3(1.f), true);
			&lightManager.addPointLight(lightTransform3, glm::vec3(lightIntensity *0.996, lightIntensity *0.535, lightIntensity*0.379), config);

			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(4.f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(0.01f));
			MeshRenderer& sponz = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/sponza/sponza.obj"),
				meshTransform6, CullingMode::cullFront, "Sponza");
			scene.addMeshRenderer(sponz);

		}

		virtual void draw(const SceneCamera& camera) {

		}

		virtual void quit() {}
	};
}



void SponzaScene::draw() {
	
	RenderWindow window = RenderWindow("geeL", 1920, 1080, WindowMode::Windowed);
	InputManager manager = InputManager();
	manager.defineButton("Forward", GLFW_KEY_W);
	manager.defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(-9.34f, -0.34f, -0.44f), vec3(-90.f, -90.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 60.f, window.width, window.height, 0.1f, 100.f);

	MaterialFactory materialFactory = MaterialFactory();
	MeshFactory meshFactory = MeshFactory(materialFactory);
	LightManager lightManager = LightManager();
	RenderPipeline shaderManager = RenderPipeline(materialFactory);
	
	RenderScene scene = RenderScene(transFactory.getWorldTransform(), lightManager, shaderManager, camera, materialFactory);
	WorldPhysics physics = WorldPhysics();
	//scene.setPhysics(&physics);

	BilateralFilter blur = BilateralFilter(1, 0.7f);
	DefaultPostProcess def = DefaultPostProcess();
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

	EnvironmentMap& preEnvMap = materialFactory.CreateEnvironmentMap("resources/hdrenv2/Arches_E_PineTree_3k.hdr");
	EnvironmentCubeMap envCubeMap = EnvironmentCubeMap(preEnvMap, cubeBuffer, 1024);
	IBLMap& iblMap = cubeMapFactory.createIBLMap(envCubeMap);

	Skybox skybox = Skybox(envCubeMap);
	scene.setSkybox(skybox);
	lightManager.addReflectionProbe(iblMap);
	
	renderer.setScene(scene);
	scene.addRequester(ssao);

	SceneControlObject& testScene = TestScene6(materialFactory, meshFactory, 
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
	
	BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
	GodRay& ray = GodRay(glm::vec3(-40, 30, -50), 35);
	BlurredPostEffect raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR();
	BlurredPostEffect ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.3f, 0.3f);
	
	DepthOfFieldBlur blur3 = DepthOfFieldBlur(2, 0.3f);
	DepthOfFieldBlurred dof = DepthOfFieldBlurred(blur3, camera.depth, 8.f, 100.f, 0.3f);

	
	postLister.add(def);
	postLister.add(ssao);

	GodRaySnippet godRaySnippet = GodRaySnippet(ray);
	renderer.addEffect(raySmooth);
	scene.addRequester(ray);
	postLister.add(raySmooth, godRaySnippet);

	//renderer.addEffect(ssrrSmooth, ssrr);
	//scene.addRequester(ssrr);
	//renderer.addEffect(dof, dof);
	//postLister.add(dof);

	FXAA fxaa = FXAA();
	//renderer.addEffect(fxaa);

	renderer.linkInformation();
	renderer.render();
}