#include "a_shadows.h"

#include <iostream>
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


#include "../renderer/cubemapping/cubemap.h"
#include "../renderer/cubemapping/skybox.h"
#include "../renderer/scene.h"

#include <glm.hpp>

#define pi 3.141592f


namespace {

	class ShadowTestObject : public SceneControlObject {

	public:

		LightManager& lightManager;
		MaterialFactory& materialFactory;
		ShaderManager& shaderManager;
		TransformFactory transformFactory;

		Model* nano;
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

			float l = 20.f;
			
			geeL::Transform* lighTransi4 = new geeL::Transform(glm::vec3(7, 5, 5), glm::vec3(-180.0f, 0, -50), glm::vec3(1.f, 1.f, 1.f));
			light = &lightManager.addLight(*lighTransi4, glm::vec3(l, l, l),
				glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.05f, 0.05f, 0.05f), 1.f, 1.f);
				
			l = 100.f;
			float angle = glm::cos(glm::radians(25.5f));
			float outerAngle = glm::cos(glm::radians(27.5f));
	
			geeL::Transform* lighTransi = new geeL::Transform(glm::vec3(-7, 5, 0), glm::vec3(-180.0f, 0, -50), glm::vec3(1.f, 1.f, 1.f));
			light = &lightManager.addLight(*lighTransi, glm::vec3(l, l, l),
				glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.05f, 0.05f, 0.05f), angle, outerAngle, 10.f);

			/*
			geeL::Transform* lighTransi3 = new geeL::Transform(glm::vec3(-7, 5, 0), glm::vec3(-180.0f, 0, -50), glm::vec3(1.f, 1.f, 1.f));
			&lightManager.addLight(*lighTransi3, glm::vec3(l, l, l),
				glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.05f, 0.05f, 0.05f), angle, outerAngle, 10.f);
			*/	
			
			/*
			l = 4.f;
			geeL::Transform* lighTransi2 = new geeL::Transform(glm::vec3(10.f, 10.f, 100.f), glm::vec3(50.0f, 0, -30), glm::vec3(1.f, 1.f, 1.f));
			dirLight = &lightManager.addLight(*lighTransi2, glm::vec3(l, l, l),
				glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.05f, 0.05f, 0.05f), 1.f);
			*/

			float height = -2.f;
			transi = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.2f, 0.2f, 0.2f));
			nanoRenderer = &scene.AddMeshRenderer("resources/nanosuit/nanosuit.obj", *transi, cullFront);

			geeL::Transform* transi2 = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.2f, 0.2f, 100.2f));
			scene.AddMeshRenderer("resources/primitives/plane.obj", *transi2, cullFront);

			geeL::Transform* transi7 = new geeL::Transform(glm::vec3(4.f, -0.4f, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 1.f, 1.f));
			scene.AddMeshRenderer("resources/cyborg/Cyborg.obj", *transi7, cullFront);
		}

		virtual void draw(const Camera& camera) {
			nanoRenderer->transform.rotate(vec3(0, 1, 0), 25);
			//light->transform.rotate(vec3(0, 0, 1), -3);
			//light->transform.translate(vec3(-0.01, 0, 0));
		}

		virtual void quit() {}
	};

}

#include "../renderer/lighting/lightmanager.h"
#include "../renderer/cameras/perspectivecamera.h"


void a_shadows() {
	RenderWindow* window = new RenderWindow("CameraTest", 1500, 850, true);
	InputManager* manager = new InputManager();
	manager->defineButton("Forward", GLFW_KEY_W);
	manager->defineButton("Forward", GLFW_KEY_A);

	geeL::Transform world = geeL::Transform(glm::vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	TransformFactory transFactory = TransformFactory(world);

	geeL::Transform& transform3 = transFactory.CreateTransform(glm::vec3(0.0f, 2.0f, 9.0f), vec3(-100.f, 0.f, 0.f), vec3(1.f, 1.f, 1.f));
	PerspectiveCamera camera3 = PerspectiveCamera(transform3, 5.f, 15.f, 65.f, window->width, window->height, 0.1f, 100.f);

	PostProcessingRenderer renderer3 = PostProcessingRenderer(window, manager);
	renderer3.init();


	DeferredRenderer renderer1 = DeferredRenderer(window, manager);
	renderer1.init();


	MaterialFactory materialFactory = MaterialFactory();
	materialFactory.setDefaultShader(true);
	MeshFactory meshFactory = MeshFactory(materialFactory);

	LightManager lightManager = LightManager();
	ShaderManager shaderManager = ShaderManager(materialFactory);
	
	RenderScene scene = RenderScene(lightManager, camera3, meshFactory);
	
	//renderer3.setScene(scene);
	renderer3.setShaderManager(shaderManager);

	renderer1.setScene(scene);
	renderer1.setShaderManager(shaderManager);

	ShadowTestObject* testObj = new ShadowTestObject(materialFactory, meshFactory, 
		lightManager, shaderManager, scene, transFactory);

	//renderer3.addObject(testObj);
	//renderer3.initObjects();

	renderer1.addObject(testObj);
	renderer1.initObjects();

	CubeMap map = CubeMap("resources/skybox2/right.jpg", "resources/skybox2/left.jpg", "resources/skybox2/top.jpg",
		"resources/skybox2/bottom.jpg", "resources/skybox2/back.jpg", "resources/skybox2/front.jpg");

	Skybox skybox = Skybox(map);
	scene.setSkybox(skybox);

	GodRay ray = GodRay(scene, glm::vec3(-40, 30, -50));
	renderer3.setEffect(ray);


	//renderer3.render();

	renderer1.render();

	delete testObj;
	delete window;
	delete manager;
}