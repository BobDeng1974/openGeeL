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
#include "../renderer/meshes/meshfactory.h"

#include "../renderer/renderer/postrenderer.h"
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

#include <glm.hpp>
#include "a_shadows.h"


using namespace geeL;

#define pi 3.141592f


SpotLight* spotLight = nullptr;

namespace {

	class ShadowTestObject : public SceneControlObject {

	public:

		LightManager& lightManager;
		MaterialFactory& materialFactory;
		ShaderManager& shaderManager;
		TransformFactory transformFactory;

		MeshRenderer* nanoRenderer;
		MeshFactory& meshFactory;
		Light* light;
		DirectionalLight* dirLight;

		Material* material;

		geeL::Transform* transi;


		ShadowTestObject(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			ShaderManager& shaderManager, RenderScene& scene, TransformFactory& transformFactory) 
			: 
			SceneControlObject(scene),
			materialFactory(materialFactory),
			meshFactory(meshFactory), 
			lightManager(lightManager),
			shaderManager(shaderManager),
			transformFactory(transformFactory)
			{}


		virtual void init() {

			float l = 100.f;
			
			geeL::Transform* lighTransi4 = new geeL::Transform(glm::vec3(7, 5, 5), glm::vec3(-180.0f, 0, -50), glm::vec3(1.f, 1.f, 1.f));
			light = &lightManager.addPointLight(*lighTransi4, glm::vec3(l *0.996 , l *0.535 , l*0.379));

			l = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			geeL::Transform* lighTransi3 = new geeL::Transform(glm::vec3(-9, 5, 0), glm::vec3(-264.0f, 0, -5), glm::vec3(1.f, 1.f, 1.f));
			spotLight = &lightManager.addSpotlight(*lighTransi3, glm::vec3(l, l, l * 2), angle, outerAngle);

			l = 0.5f;
			geeL::Transform* lighTransi2 = new geeL::Transform(glm::vec3(0.f, 0.f, 0.f), glm::vec3(75, 20, 10), glm::vec3(1.f, 1.f, 1.f));
			//dirLight = &lightManager.addDirectionalLight(*lighTransi2, glm::vec3(l, l, l));

			float height = -2.f;
			transi = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.2f, 0.2f, 0.2f));
			nanoRenderer = &scene.AddMeshRenderer("resources/nanosuit/nanosuit.obj", *transi, cullFront, true, "Nanosuit");

			geeL::Transform* transi2 = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.2f, 0.2f, 100.2f));
			MeshRenderer& plane = scene.AddMeshRenderer("resources/primitives/plane.obj", *transi2, cullFront, true, "Floor");

			for (auto it = plane.deferredMaterialsBegin(); it != plane.deferredMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterialContainer* defmat = dynamic_cast<DefaultMaterialContainer*>(&mat->container);

				if (defmat != nullptr) {
					defmat->setRoughness(0.9f);
					defmat->setMetallic(0.f);
					defmat->setColor(vec3(0.4f, 0.4f, 0.4f));
				}
			}

			geeL::Transform* transi5 = new geeL::Transform(glm::vec3(-30.0f, -4.f, 25.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.3f, 0.3f, 0.3f));
			MeshRenderer& box1 = scene.AddMeshRenderer("resources/empire/EmpireState_lp.obj", *transi5, cullFront, true, "Empire State");

			for (auto it = box1.deferredMaterialsBegin(); it != box1.deferredMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterialContainer* defmat = dynamic_cast<DefaultMaterialContainer*>(&mat->container);

				if (defmat != nullptr) {
					defmat->setRoughness(0.5f);
					defmat->setColor(vec3(0.5f, 0.5f, 0.5f));
				}
			}

			geeL::Transform* transi7 = new geeL::Transform(glm::vec3(8.f, 0.f, 4.f), glm::vec3(0.f), glm::vec3(1.f, 1.f, 1.f));
			MeshRenderer& sphere1 = scene.AddMeshRenderer("resources/primitives/sphere.obj", *transi7, cullFront, true, "Sphere");

			for (auto it = sphere1.deferredMaterialsBegin(); it != sphere1.deferredMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterialContainer* defmat = dynamic_cast<DefaultMaterialContainer*>(&mat->container);

				if (defmat != nullptr) {
					defmat->setRoughness(0.f);
					defmat->setMetallic(0.5f);
					//defmat->setColor(vec3(0.1f, 0.1f, 0.1f));
				}
			}

			geeL::Transform* transi3 = new geeL::Transform(glm::vec3(0.0f, 1, -2.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(5.2f, 2.2f, 1.2f));
			MeshRenderer& box = scene.AddMeshRenderer("resources/primitives/cube.obj", *transi3, cullFront, true, "Box");

			for (auto it = box.deferredMaterialsBegin(); it != box.deferredMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterialContainer* defmat = dynamic_cast<DefaultMaterialContainer*>(&mat->container);

				if (defmat != nullptr) {
					defmat->setRoughness(0.0f);
					defmat->setMetallic(0.1f);
					defmat->setColor(vec3(0.1f, 0.1f, 0.1f));
				}
			}

			geeL::Transform* transi79 = new geeL::Transform(glm::vec3(4.f, -0.4f, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f));
			scene.AddMeshRenderer("resources/cyborg/Cyborg.obj", *transi79, cullFront, true, "Cyborg");
		}

		virtual void draw(const Camera& camera) {
			nanoRenderer->transform.rotate(vec3(0, 1, 0), 2000 * Time::deltaTime);
			//light->transform.rotate(vec3(0, 0, 1), -3);
			//light->transform.translate(vec3(-0.01, 0, 0));
		}

		virtual void quit() {}
	};

}

#include "../renderer/lighting/lightmanager.h"
#include "../renderer/cameras/perspectivecamera.h"


void a_shadows() {
	
	RenderWindow window = RenderWindow("geeL", 1920, 1080, true);
	InputManager manager = InputManager();
	manager.defineButton("Forward", GLFW_KEY_W);
	manager.defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& transform3 = transFactory.CreateTransform(glm::vec3(0.0f, 2.0f, 9.0f), vec3(-100.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera3 = PerspectiveCamera(transform3, 5.f, 15.f, 60.f, window.width, window.height, 0.1f, 100.f);

	BilateralFilter blur = BilateralFilter(1, 0.3f);
	DefaultPostProcess def = DefaultPostProcess();
	SSAO ssao = SSAO(camera3, blur, 10.f);
	RenderContext context = RenderContext();
	DeferredRenderer& renderer1 = DeferredRenderer(window, manager, context, def, &ssao, 0.6f);
	renderer1.init();

	MaterialFactory materialFactory = MaterialFactory();
	materialFactory.setDefaultShader(true);
	MeshFactory meshFactory = MeshFactory(materialFactory);

	LightManager lightManager = LightManager(vec3(0.15f));
	ShaderManager shaderManager = ShaderManager(materialFactory);
	
	RenderScene scene = RenderScene(lightManager, camera3, meshFactory);

	TexturedCubeMap map = TexturedCubeMap("resources/skybox2/right.jpg", "resources/skybox2/left.jpg", "resources/skybox2/top.jpg",
		"resources/skybox2/bottom.jpg", "resources/skybox2/back.jpg", "resources/skybox2/front.jpg");

	EnvironmentMap envMap = EnvironmentMap("resources/hdrenv2/Arches_E_PineTree_3k.hdr");
	EnvironmentCubeMap envCubeMap = EnvironmentCubeMap(envMap, 1024);
	IrradianceMap irrMap = IrradianceMap(envCubeMap);

	Skybox skybox = Skybox(irrMap);
	scene.setSkybox(skybox);
	
	renderer1.setScene(scene);
	renderer1.setShaderManager(shaderManager);

	ShadowTestObject* testObj = new ShadowTestObject(materialFactory, meshFactory, 
		lightManager, shaderManager, scene, transFactory);

	renderer1.addObject(testObj);
	renderer1.initObjects();

	GUIRenderer gui = GUIRenderer(window, context);
	ObjectLister objectLister = ObjectLister(scene, window, 0.01f, 0.01f, 0.17f, 0.3f);
	objectLister.add(camera3);
	gui.addElement(objectLister);
	PostProcessingEffectLister postLister = PostProcessingEffectLister(window, 0.01f, 0.35f, 0.17f, 0.3f);
	gui.addElement(postLister);
	SystemInformation sysInfo = SystemInformation(renderer1.getRenderTime(), window, 0.01f, 0.7f, 0.17f);
	gui.addElement(sysInfo);

	renderer1.addGUIRenderer(&gui);
	
	BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
	GodRay& ray = GodRay(scene, glm::vec3(-40, 30, -50), 15.f);
	BlurredPostEffect raySmooth = BlurredPostEffect(ray, blur2, 0.2f, 0.2f);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR(camera3);
	BlurredPostEffect ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.3f, 0.3f);
	
	DepthOfFieldBlur blur3 = DepthOfFieldBlur(2, 0.3f);
	DepthOfFieldBlurred dof = DepthOfFieldBlurred(blur3, camera3.depth, 2.f, 100.f, 0.3f);

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
	renderer1.addEffect(volSmooth, { &vol, &sobelBlur });
	postLister.add(volSmooth, lightSnippet);

	//renderer1.addEffect(bloom);
	//postLister.add(bloom);

	//GodRaySnippet godRaySnippet = GodRaySnippet(ray);
	//renderer1.addEffect(raySmooth);
	//postLister.add(raySmooth, godRaySnippet);

	renderer1.addEffect(ssrrSmooth, ssrr);
	
	renderer1.addEffect(dof, dof);
	postLister.add(dof);

	renderer1.addEffect(colorCorrect);
	postLister.add(colorCorrect);

	renderer1.addEffect(fxaa);

	renderer1.linkInformation();
	renderer1.render();

	delete testObj;
}