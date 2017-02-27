#include "a_shadows.h"

#include <iostream>
#include <vector>
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

#include "../renderer/cubemapping/cubemap.h"
#include "../renderer/cubemapping/skybox.h"
#include "../renderer/scene.h"
#include "../renderer/utility/rendertime.h"

#include <glm.hpp>

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

			float l = 200.f;
			
			geeL::Transform* lighTransi4 = new geeL::Transform(glm::vec3(7, 5, 5), glm::vec3(-180.0f, 0, -50), glm::vec3(1.f, 1.f, 1.f));
			light = &lightManager.addPointLight(*lighTransi4, glm::vec3(l *0.996 , l *0.535 , l*0.379));
				
			l = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));

			geeL::Transform* lighTransi3 = new geeL::Transform(glm::vec3(-7, 5, 0), glm::vec3(-180.0f, 0, -50), glm::vec3(1.f, 1.f, 1.f));
			spotLight = &lightManager.addSpotlight(*lighTransi3, glm::vec3(l, l, l * 2), angle, outerAngle);
			
			l = 0.5f;
			geeL::Transform* lighTransi2 = new geeL::Transform(glm::vec3(0.f, 0.f, 0.f), glm::vec3(75, 20, 10), glm::vec3(1.f, 1.f, 1.f));
			//dirLight = &lightManager.addDirectionalLight(*lighTransi2, glm::vec3(l, l, l));
			

			float height = -2.f;
			transi = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.2f, 0.2f, 0.2f));
			nanoRenderer = &scene.AddMeshRenderer("resources/nanosuit/nanosuit.obj", *transi, cullFront);

			geeL::Transform* transi2 = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.2f, 0.2f, 100.2f));
			MeshRenderer& plane = scene.AddMeshRenderer("resources/primitives/plane.obj", *transi2, cullFront);

			for (auto it = plane.defaultMaterialsBegin(); it != plane.defaultMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterial* defmat = dynamic_cast<DefaultMaterial*>(mat);

				if (defmat != nullptr) {
					defmat->setRoughness(0.2f);
					defmat->setColor(vec3(0.4f, 0.4f, 0.4f));
				}
			}

			geeL::Transform* transi5 = new geeL::Transform(glm::vec3(-30.0f, -4.f, 25.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.3f, 0.3f, 0.3f));
			MeshRenderer& box1 = scene.AddMeshRenderer("resources/empire/EmpireState_lp.obj", *transi5, cullFront);

			for (auto it = box1.defaultMaterialsBegin(); it != box1.defaultMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterial* defmat = dynamic_cast<DefaultMaterial*>(mat);

				if (defmat != nullptr) {
					defmat->setRoughness(0.9f);
					defmat->setColor(vec3(0.5f, 0.5f, 0.5f));
				}
			}

			geeL::Transform* transi7 = new geeL::Transform(glm::vec3(8.f, 0.f, 4.f), glm::vec3(0.f), glm::vec3(1.f, 1.f, 1.f));
			MeshRenderer& sphere1 = scene.AddMeshRenderer("resources/primitives/sphere.obj", *transi7, cullFront);

			for (auto it = sphere1.defaultMaterialsBegin(); it != sphere1.defaultMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterial* defmat = dynamic_cast<DefaultMaterial*>(mat);

				if (defmat != nullptr) {
					defmat->setRoughness(0.f);
					defmat->setMetallic(0.1f);
				}
			}

			geeL::Transform* transi3 = new geeL::Transform(glm::vec3(0.0f, 1, -2.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(5.2f, 2.2f, 1.2f));
			MeshRenderer& box = scene.AddMeshRenderer("resources/primitives/cube.obj", *transi3, cullFront);

			for (auto it = box.defaultMaterialsBegin(); it != box.defaultMaterialsEnd(); it++) {
				Material* mat = it->second;
				DefaultMaterial* defmat = dynamic_cast<DefaultMaterial*>(mat);

				if (defmat != nullptr) {
					defmat->setRoughness(0.f);
					defmat->setMetallic(0.1f);
					defmat->setColor(vec3(0.1f, 0.1f, 0.1f));
				}
			}

			GenericMaterial* custom = &materialFactory.CreateMaterial(materialFactory.getForwardShader());
			custom->addParameter("shininess", 64.f);
			std::vector<Material*> materials;
			materials.push_back(custom);

			geeL::Transform* transi79 = new geeL::Transform(glm::vec3(4.f, -0.4f, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f));
			scene.AddMeshRenderer("resources/cyborg/Cyborg.obj", *transi79, cullFront);
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
	RenderWindow* window = new RenderWindow("geeL", 1920, 1080, true);
	InputManager* manager = new InputManager();
	manager->defineButton("Forward", GLFW_KEY_W);
	manager->defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& transform3 = transFactory.CreateTransform(glm::vec3(0.0f, 2.0f, 9.0f), vec3(-100.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera3 = PerspectiveCamera(transform3, 5.f, 15.f, 55.f, window->width, window->height, 0.1f, 100.f);

	BilateralFilter blur = BilateralFilter(1, 0.2f);
	SSAO ssao = SSAO(camera3, blur, 10.f);
	DeferredRenderer& renderer1 = DeferredRenderer(window, manager, &ssao, 0.6f);
	renderer1.init();

	MaterialFactory materialFactory = MaterialFactory();
	materialFactory.setDefaultShader(true);
	MeshFactory meshFactory = MeshFactory(materialFactory);

	LightManager lightManager = LightManager();
	ShaderManager shaderManager = ShaderManager(materialFactory);
	
	RenderScene scene = RenderScene(lightManager, camera3, meshFactory);

	CubeMap map = CubeMap("resources/skybox2/right.jpg", "resources/skybox2/left.jpg", "resources/skybox2/top.jpg",
		"resources/skybox2/bottom.jpg", "resources/skybox2/back.jpg", "resources/skybox2/front.jpg");

	Skybox skybox = Skybox(map);
	scene.setSkybox(skybox);

	renderer1.setScene(scene);
	renderer1.setShaderManager(shaderManager);

	ShadowTestObject* testObj = new ShadowTestObject(materialFactory, meshFactory, 
		lightManager, shaderManager, scene, transFactory);

	renderer1.addObject(testObj);
	renderer1.initObjects();
	
	BilateralFilter& blur2 = BilateralFilter(1, 0.1f);
	GodRay& ray = GodRay(scene, glm::vec3(-40, 30, -50), 15.f);
	BlurredPostEffect raySmooth = BlurredPostEffect(ray, blur2, 0.3f);

	GaussianBlur& blur4 = GaussianBlur();
	SSRR& ssrr = SSRR(camera3);
	BlurredPostEffect ssrrSmooth = BlurredPostEffect(ssrr, blur4, 0.3f);
	
	DepthOfFieldBlur blur3 = DepthOfFieldBlur(3, 0.3f);
	DepthOfFieldBlurred dof = DepthOfFieldBlurred(blur3, camera3.depth, 12.f, 100.f, 0.4f);

	FXAA fxaa = FXAA();

	GaussianBlur& blur5 = GaussianBlur();
	Bloom bloom = Bloom(blur5, 0.7f, 0.2f);

	FXAA& fxaa2 = FXAA();
	VolumetricLight vol = VolumetricLight(scene, *spotLight, 0.3f, 1.f, 160);
	BlurredPostEffect volSmooth = BlurredPostEffect(vol, fxaa2, 0.4f);

	SobelFilter sobel = SobelFilter();
	SobelBlur sobelBlur = SobelBlur(sobel);

	renderer1.addEffect(sobelBlur);
	//renderer1.addEffect(volSmooth, vol);
	//renderer1.addEffect(bloom);
	//renderer1.addEffect(ssrrSmooth, ssrr);
	//renderer1.addEffect(raySmooth);
	renderer1.addEffect(dof, dof);
	renderer1.addEffect(fxaa);
	
	renderer1.linkInformation();
	renderer1.render();

	delete testObj;
	delete window;
	delete manager;

}