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
#include "../renderer/texturing/simpletexture.h"
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
#include "../renderer/shader/shadermanager.h"
#include "../renderer/meshes/mesh.h"
#include "../renderer/meshes/model.h"
#include "../renderer/meshes/meshrenderer.h"
#include "../renderer/meshes/skinnedrenderer.h"
#include "../renderer/meshes/meshfactory.h"

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
#include "../renderer/cubemapping/skybox.h"
#include "../renderer/scene.h"
#include "../renderer/utility/rendertime.h"

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

#include "testscene.h"

#define pi 3.141592f

using namespace geeL;


SpotLight* spotLight = nullptr;

namespace {

	class TestScene : public SceneControlObject {

	public:
		LightManager& lightManager;
		MaterialFactory& materialFactory;
		ShaderManager& shaderManager;
		TransformFactory transformFactory;
		MeshFactory& meshFactory;
		Physics* physics;

		MeshRenderer* nanoRenderer;


		TestScene(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			ShaderManager& shaderManager, RenderScene& scene, TransformFactory& transformFactory, Physics* physics)
				: SceneControlObject(scene),
					materialFactory(materialFactory),
					meshFactory(meshFactory), 
					lightManager(lightManager),
					shaderManager(shaderManager),
					transformFactory(transformFactory),
					physics(physics) {}


		virtual void init() {

			float lightIntensity = 100.f;

			Transform& lightTransform1 = transformFactory.CreateTransform(vec3(7, 5, 5), vec3(-180.0f, 0, -50), vec3(1.f, 1.f, 1.f));
			&lightManager.addPointLight(lightTransform1, glm::vec3(lightIntensity *0.996 , lightIntensity *0.535 , lightIntensity*0.379));

			lightIntensity = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			SimpleTexture& texture = materialFactory.CreateTexture("resources/textures/cookie.png", 
				ColorType::GammaSpace, WrapMode::Repeat, FilterMode::Linear);

			Transform& lightTransform2 = transformFactory.CreateTransform(vec3(-9, 5, 0), vec3(-264.0f, 0, -5), vec3(1.f, 1.f, 1.f));
			spotLight = &lightManager.addSpotlight(lightTransform2, glm::vec3(lightIntensity, lightIntensity, lightIntensity * 2), angle, outerAngle);
			spotLight->setLightCookie(texture);

			lightIntensity = 0.5f;
			//geeL::Transform& lightTransform3 = transformFactory.CreateTransform(vec3(0.f, 0.f, 0.f), vec3(75, 20, 10), vec3(1.f, 1.f, 1.f));
			//&lightManager.addDirectionalLight(lightTransform3, glm::vec3(l, l, l));

			float height = -2.f;
			Transform& meshTransform1 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(0.2f, 0.2f, 0.2f));
			
			nanoRenderer = &meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/nanosuit/nanosuit.obj"), 
				meshTransform1, CullingMode::cullFront, "MeshRenderer");
			scene.AddMeshRenderer(*nanoRenderer);


			Transform& meshTransform2 = transformFactory.CreateTransform(vec3(0.0f, height, 0.0f), vec3(0.f, 0.f, 0.f), vec3(100.2f, 0.2f, 100.2f));
			MeshRenderer& plane = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/plane.obj"),
				meshTransform2, CullingMode::cullFront, "Floor");

			scene.AddMeshRenderer(plane);
			if (physics != nullptr) physics->addPlane(vec3(0.f, 1.f, 0.f), meshTransform2, RigidbodyProperties(0.f, false));

			for (auto it = plane.deferredMaterialsBegin(); it != plane.deferredMaterialsEnd(); it++) {
				MaterialContainer& container = it->second->container;
				container.setFloatValue("Roughness", 0.9f);
				container.setFloatValue("Metallic", 0.f);
				container.setVectorValue("Color", vec3(0.4f, 0.4f, 0.4f));
			}


			Transform& meshTransform3 = transformFactory.CreateTransform(vec3(-9.f, -3.f, 11.0f), vec3(0.5f, 0.5f, 0.5f), vec3(0.3f, 0.3f, 0.3f));
			MeshRenderer& box1 = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/empire/EmpireState_lp.obj"),
				meshTransform3, CullingMode::cullFront, "Empire State");
			scene.AddMeshRenderer(box1);

			for (auto it = box1.deferredMaterialsBegin(); it != box1.deferredMaterialsEnd(); it++) {
				MaterialContainer& container = it->second->container;
				container.setFloatValue("Roughness", 0.5f);
				container.setVectorValue("Color", vec3(0.5f, 0.5f, 0.5f));
			}

			Transform& meshTransform4 = transformFactory.CreateTransform(vec3(8.f, 5.f, 4.f), vec3(0.f), vec3(1.f, 1.f, 1.f));
			MeshRenderer& sphere1 = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/sphere.obj"),
				meshTransform4, CullingMode::cullFront, "Sphere");
			scene.AddMeshRenderer(sphere1);
			//if (physics != nullptr) physics->addSphere(1.f, meshTransform4, RigidbodyProperties(10.f, false));
			//if (physics != nullptr) physics->addMesh(sphere1.getModel(), meshTransform4, RigidbodyProperties(10.f, false));

			for (auto it = sphere1.deferredMaterialsBegin(); it != sphere1.deferredMaterialsEnd(); it++) {
				MaterialContainer& container = it->second->container;
				container.setFloatValue("Roughness", 0.f);
				container.setFloatValue("Metallic", 0.5f);
			}

			Transform& meshTransform5 = transformFactory.CreateTransform(vec3(0.0f, 0.5f, -2.0f), vec3(0.5f, 0.5f, 0.5f), vec3(5.2f, 2.2f, 1.2f));
			MeshRenderer& box = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/primitives/cube.obj"),
				meshTransform5, CullingMode::cullFront, "Box");
			scene.AddMeshRenderer(box);

			for (auto it = box.deferredMaterialsBegin(); it != box.deferredMaterialsEnd(); it++) {
				MaterialContainer& container = it->second->container;
				container.setFloatValue("Roughness", 0.f);
				container.setFloatValue("Metallic", 0.1f);
				container.setVectorValue("Color", vec3(0.1f, 0.1f, 0.1f));
			}

			Transform& meshTransform6 = transformFactory.CreateTransform(vec3(4.f, -2.f, 0.0f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
			MeshRenderer& cyborg = meshFactory.CreateMeshRenderer(meshFactory.CreateStaticModel("resources/cyborg/Cyborg.obj"),
				meshTransform6, CullingMode::cullFront, "Cyborg");
			scene.AddMeshRenderer(cyborg);

			/*
			float scale = 0.05f;
			Transform& meshTransform7 = transformFactory.CreateTransform(vec3(2.f, -2.f, 4.0f), vec3(-90.f, 0.f, 0.f), vec3(scale, scale, scale));
			SkinnedMeshRenderer& dude = meshFactory.CreateSkinnedMeshRenderer(meshFactory.CreateSkinnedModel("resources/guard/boblampclean.md5mesh"),
				meshTransform7, CullingMode::cullFront, "Dude");
			scene.AddMeshRenderer(dude);

			SimpleAnimator& anim = dude.addComponent(SimpleAnimator(dude.getSkinnedModel(), dude.getSkeleton()));
			anim.playAnimation(0);
			*/
		}

		virtual void draw(const Camera& camera) {
			nanoRenderer->transform.rotate(vec3(0.f, 1.f, 0.f), 1.5f * Time::deltaTime);
		}

		virtual void quit() {}
	};

}



void draw() {
	
	RenderWindow window = RenderWindow("geeL", 1920, 1080, WindowMode::Windowed);
	InputManager manager = InputManager();
	manager.defineButton("Forward", GLFW_KEY_W);
	manager.defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& cameraTransform = Transform(vec3(0.0f, 2.0f, 9.0f), vec3(-90.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera = PerspectiveCamera(cameraTransform, 5.f, 0.45f, 60.f, window.width, window.height, 0.1f, 100.f);

	MaterialFactory materialFactory = MaterialFactory();
	MeshFactory meshFactory = MeshFactory(materialFactory);

	BilateralFilter blur = BilateralFilter(1, 0.5f);
	DefaultPostProcess def = DefaultPostProcess();
	SSAO ssao = SSAO(blur, 10.f);
	RenderContext context = RenderContext();
	DeferredRenderer& renderer = DeferredRenderer(window, manager, context, def, materialFactory);
	renderer.addSSAO(ssao, 0.5f);
	renderer.init();
	

	LightManager lightManager = LightManager(vec3(0.15f));
	ShaderManager shaderManager = ShaderManager(materialFactory);
	
	RenderScene scene = RenderScene(lightManager, camera, meshFactory, transFactory.getWorldTransform());
	WorldPhysics physics = WorldPhysics();
	scene.setPhysics(&physics);

	EnvironmentMap& envMap = materialFactory.CreateEnvironmentMap("resources/hdrenv2/Arches_E_PineTree_3k.hdr");
	EnvironmentCubeMap envCubeMap = EnvironmentCubeMap(envMap, 1024);
	IrradianceMap irrMap = IrradianceMap(envCubeMap);

	Skybox skybox = Skybox(irrMap);
	scene.setSkybox(skybox);
	
	renderer.setScene(scene);
	renderer.setShaderManager(shaderManager);

	TestScene testScene = TestScene(materialFactory, meshFactory, 
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
	GodRay& ray = GodRay(scene, glm::vec3(-40, 30, -50), 15.f);
	BlurredPostEffect raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR();
	BlurredPostEffect ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.3f, 0.3f);
	
	DepthOfFieldBlur blur3 = DepthOfFieldBlur(2, 0.3f);
	DepthOfFieldBlurred dof = DepthOfFieldBlurred(blur3, camera.depth, 14.f, 100.f, 0.3f);

	FXAA fxaa = FXAA();

	BloomFilter filter = BloomFilter();
	GaussianBlur& blur5 = GaussianBlur();
	Bloom bloom = Bloom(filter, blur5, 0.4f, 0.2f);

	SobelFilter sobel = SobelFilter(15);
	SobelBlur sobelBlur = SobelBlur(sobel);
	VolumetricLight vol = VolumetricLight(scene, *spotLight, 0.3f, 1.f, 160);
	BlurredPostEffect volSmooth = BlurredPostEffect(vol, sobelBlur, 0.4f, 0.4f);

	ColorCorrection colorCorrect = ColorCorrection();

	postLister.add(def);
	postLister.add(ssao);

	VolumetricLightSnippet lightSnippet = VolumetricLightSnippet(vol);
	renderer.addEffect(volSmooth, { &vol, &sobelBlur });
	postLister.add(volSmooth, lightSnippet);

	//renderer.addEffect(bloom);
	//postLister.add(bloom);

	//GodRaySnippet godRaySnippet = GodRaySnippet(ray);
	//renderer.addEffect(raySmooth);
	//postLister.add(raySmooth, godRaySnippet);

	renderer.addEffect(ssrrSmooth, ssrr);

	//renderer.addEffect(dof, dof);
	//postLister.add(dof);

	//renderer.addEffect(colorCorrect);
	//postLister.add(colorCorrect);

	//renderer.addEffect(fxaa);

	renderer.linkInformation();
	renderer.render();
}