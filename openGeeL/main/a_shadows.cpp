#include "a_shadows.h"

#include <iostream>
#include "../renderer/cameras/camera.h"
#include "../renderer/cameras/perspectivecamera.h"
#include "../renderer/lighting/light.h"
#include "../renderer/lighting/lightmanager.h"
#include "../renderer/lighting/pointlight.h"
#include "../renderer/lighting/directionallight.h"

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

#include "../renderer/cubemapping/cubemap.h"
#include "../renderer/cubemapping/skybox.h"
#include "../renderer/scene.h"

#define pi 3.141592f

namespace {

	class ShadowTestObject : public SceneObject {

	public:

		Shader* shader;
		LightManager& lightManager;
		MaterialFactory& materialFactory;
		ShaderManager& shaderManager;

		Model* nano;
		MeshRenderer* nanoRenderer;
		MeshFactory& meshFactory;
		//MeshDrawer& meshDrawer;
		//RenderScene& scene;

		Material* material;


		ShadowTestObject(MaterialFactory& materialFactory, MeshFactory& meshFactory, LightManager& lightManager,
			ShaderManager& shaderManager, RenderScene& scene) 
			: 
			SceneObject(scene),
			materialFactory(materialFactory),
			meshFactory(meshFactory), 
			lightManager(lightManager),
			shaderManager(shaderManager)
			{}


		virtual void init() {

			shader = &materialFactory.CreateShader("renderer/shaders/reflective.vert", "renderer/shaders/reflective.frag");

			//PointLight* light1 = new PointLight(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.5f, 0.5f, 0.5f),
			//	glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.2f, 0.2f, 0.2f), 1.f, 0.69f, 0.032f);
			//PointLight* light2 = new PointLight(glm::vec3(-2.0f, 2.0f, -7.0f), glm::vec3(0.3f, 0.3f, 0.9f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f));

			float l = 1.f;
			lightManager.addLight(-glm::vec3(-1.2f, -1.0f, -2.0f), glm::vec3(l, l, l),
				glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.05f, 0.05f, 0.05f), 1.f);

			float height = -2.f;
			geeL::Transform* transi = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.2f, 0.2f, 0.2f));
			nanoRenderer = &scene.AddMeshRenderer("resources/nanosuit/nanosuit.obj", *transi, cullFront);
			
			geeL::Transform* transi2 = new geeL::Transform(glm::vec3(0.0f, height, 0.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(100.2f, 0.2f, 100.2f));
			scene.AddMeshRenderer("resources/primitives/plane.obj", *transi2, cullFront);
			
		}

		virtual void draw(const Camera& camera) {
			nanoRenderer->transform.rotate(vec3(0, 1, 0), 25);
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


	MaterialFactory materialFactory = MaterialFactory();
	MeshFactory meshFactory = MeshFactory(materialFactory);

	LightManager lightManager = LightManager();
	ShaderManager shaderManager = ShaderManager(materialFactory);
	
	RenderScene scene = RenderScene(lightManager, camera3, meshFactory);
	
	renderer3.setScene(scene);
	renderer3.setShaderManager(shaderManager);

	ShadowTestObject* testObj = new ShadowTestObject(materialFactory, meshFactory, 
		lightManager, shaderManager, scene);

	renderer3.addObject(testObj);
	renderer3.initObjects();

	CubeMap map = CubeMap("resources/skybox2/right.jpg", "resources/skybox2/left.jpg", "resources/skybox2/top.jpg",
		"resources/skybox2/bottom.jpg", "resources/skybox2/back.jpg", "resources/skybox2/front.jpg");

	Skybox skybox = Skybox(map);
	scene.setSkybox(skybox);

	//PostProcessingEffect& effect2 = ToneMapping(1.f);
	//renderer3.setEffect(effect2);
	
	renderer3.render();

	delete testObj;
	delete window;
	delete manager;
}