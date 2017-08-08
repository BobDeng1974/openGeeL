#include <algorithm>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "materials/material.h"
#include "meshes/mesh.h"
#include "meshes/model.h"
#include "meshes/meshrenderer.h"
#include "meshes/skinnedrenderer.h"
#include "meshes/meshfactory.h"
#include "materials/materialfactory.h"
#include "cameras/camera.h"
#include "cubemapping/skybox.h"
#include "utility/screeninfo.h"
#include "lights/light.h"
#include "lights/lightmanager.h"
#include "pipeline.h"
#include "inputmanager.h"
#include "transformation/transform.h"
#include "utility/worldinformation.h"
#include "renderscene.h"

using namespace std;

namespace geeL {

	Scene::Scene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline, SceneCamera& camera)
		: lightManager(lightManager), pipeline(pipeline), camera(&camera), worldTransform(world), skybox(nullptr) {
	
		addRequester(lightManager);
	}


	void Scene::addRequester(SceneRequester& requester) {
		sceneRequester.push_back(&requester);

		requester.updateSkybox(*skybox);
		requester.updateCamera(*camera);
	}

	void Scene::setCamera(SceneCamera& camera) {
		this->camera = &camera;

		for (auto it = sceneRequester.begin(); it != sceneRequester.end(); it++) {
			SceneRequester& requester = **it;
			requester.updateCamera(*this->camera);
		}
	}

	const SceneCamera& Scene::getCamera() const {
		return *camera;
	}

	SceneCamera& Scene::getCamera() {
		return *camera;
	}

	const LightManager& Scene::getLightmanager() const {
		return lightManager;
	}

	LightManager& Scene::getLightmanager() {
		return lightManager;
	}

	void Scene::addShader(SceneShader& shader) {
		auto it = renderObjects.find(&shader);
		if (it == renderObjects.end()) {
			pipeline.staticBind(*camera, lightManager, shader);
			if(shader.getUseLight()) lightManager.addShaderListener(shader);
		}
	}

	void Scene::addMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			//Init shader if it hasn't been added to the scene yet
			addShader(shader);

			renderer.addMaterialChangeListener([this](MeshRenderer& renderer, Material oldMaterial, Material newMaterial) {
				updateMeshRenderer(renderer, oldMaterial, newMaterial);
			});

			renderObjects[&shader][renderer.transform.getID()] = &renderer;
		});
	}

	void Scene::removeMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			auto shaders = renderObjects.find(&shader);
			if (shaders != renderObjects.end()) {
				auto& objects = (*shaders).second;

				auto obj = objects.find(renderer.transform.getID());
				if (obj != objects.end())
					objects.erase(obj);
			}
		});

	}


	void Scene::updateMeshRenderer(MeshRenderer& renderer, Material oldMaterial, Material newMaterial) {
		SceneShader& oldShader = oldMaterial.getShader();
		SceneShader& newShader = newMaterial.getShader();

		if (&oldShader == &newShader) return;

		//Remove mesh renderer from old materials shaders bucket
		auto shaders = renderObjects.find(&oldShader);
		if (shaders != renderObjects.end()) {
			auto& objects = (*shaders).second;

			auto obj = objects.find(renderer.transform.getID());
			if (obj != objects.end())
				objects.erase(obj);
		}

		renderObjects[&newShader][renderer.transform.getID()] = &renderer;
	}


	void Scene::setSkybox(Skybox& skybox) {
		this->skybox = &skybox;

		for (auto it = sceneRequester.begin(); it != sceneRequester.end(); it++) {
			SceneRequester& requester = **it;
			requester.updateSkybox(*this->skybox);
		}

		//Redraw reflection probes since skybox is also visible in them
		lightManager.drawReflectionProbes();
	}




	RenderScene::RenderScene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline,
		SceneCamera& camera, const MaterialFactory& materialFactory, Input& input)
			: Scene(world, lightManager, pipeline, camera), 
				materialFactory(materialFactory), input(input) {
	
		addShader(materialFactory.getForwardShader());
	}

	
	void RenderScene::init() {
		if (!initialized) {
			lightManager.drawShadowmapsForced(*this, camera);
			initialized = true;
		}
	}

	void RenderScene::updateProbes() {
		lightManager.drawReflectionProbes();
	}

	void RenderScene::updateCamera() {
		pipeline.bindCamera(*camera);
	}

	void RenderScene::setCamera(SceneCamera& camera) {
		Scene::setCamera(camera);
		updateCamera();
	}

	void RenderScene::run() {
		lock();

		iterSceneObjects([&](SceneObject& obj) {
			obj.update(input);
		});

		worldTransform.update();

		iterSceneObjects([&](SceneObject& obj) {
			obj.lateUpdate();
		});

		unlock();
	}


	void RenderScene::draw(SceneShader& shader) {
		pipeline.dynamicBind(lightManager, shader, *camera);

		iterRenderObjects(shader, [&](const MeshRenderer& object) {
			if (object.isActive())
				object.draw();
		});
	}

	void RenderScene::drawDefault() const {
		drawDefault(*camera);
	}

	void RenderScene::drawDefault(const Camera& camera) const {

		//Only bind camera if it is external since the scene 
		//camera was already bound during update process 
		if (&camera != this->camera)
			pipeline.bindCamera(camera);

		SceneShader* shader = &materialFactory.getDeferredShader();
		pipeline.dynamicBind(lightManager, *shader, camera);
		iterRenderObjects(*shader, [&](const MeshRenderer& object) {
			if (object.isActive())
				object.draw(*shader);
		});

		shader = &materialFactory.getDefaultShader(DefaultShading::DeferredSkinned);
		pipeline.dynamicBind(lightManager, *shader, camera);
		iterRenderObjects(*shader, [&](const MeshRenderer& object) {
			if (object.isActive())
				object.draw(*shader);
		});
	}

	void RenderScene::drawDefaultForward(const Camera& camera) const {
		SceneShader& shader = materialFactory.getForwardShader();

		shader.bind<glm::mat4>("projection", camera.getProjectionMatrix());
		shader.bind<glm::vec3>("cameraPosition", camera.transform.getPosition());

		drawObjects(shader, &camera);
	}

	void RenderScene::drawGeneric() const {
		drawGeneric(*camera);
	}

	void RenderScene::drawGeneric(const Camera& camera) const {

		//Only bind camera if it is external since the scene 
		//camera was already bound during update process 
		if (&camera != this->camera)
			pipeline.bindCamera(camera);

		//Draw all registered objects with shaders other than deferred shader
		const SceneShader& defShader = materialFactory.getDeferredShader();
		const SceneShader& defSkinnedShader = materialFactory.getDefaultShader(DefaultShading::DeferredSkinned);
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			SceneShader* shader = it->first;
			if (shader == &defShader || shader == &defSkinnedShader)
				continue;

			pipeline.dynamicBind(lightManager, *shader, camera);

			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				if (object.isActive())
					object.draw(*shader);
			}
		}
	}

	void RenderScene::drawObjects(SceneShader& shader, const Camera* const camera) const {
		if (camera != nullptr)
			pipeline.dynamicBind(lightManager, shader, *camera);
		else
			pipeline.dynamicBind(lightManager, shader);

		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;

				if (object.isActive())
					object.drawExclusive(shader);
			}
		}
	}
	
	void RenderScene::drawGeometry(const RenderShader& shader) const {
		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive())
				object.drawGeometry(shader);
		});
	}

	void RenderScene::drawStaticObjects(const RenderShader& shader) const {
		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive() && object.getRenderMode() == RenderMode::Static)
				object.drawGeometry(shader);
		});
	}

	void RenderScene::drawSkinnedObjects(const RenderShader& shader) const {
		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive() && object.getRenderMode() == RenderMode::Skinned)
				object.drawGeometry(shader);
		});
	}

	

	void RenderScene::drawSkybox() const {
		if (skybox != nullptr)
			skybox->draw(*camera);
	}

	void RenderScene::drawSkybox(const Camera& camera) const {
		if (skybox != nullptr)
			skybox->draw(camera);
	}

	void RenderScene::bindSkybox(RenderShader& shader) const {
		if (skybox != nullptr)
			skybox->bind(shader);
	}

	void RenderScene::lock() {
		mutex.lock();
	}

	void RenderScene::unlock() {
		mutex.unlock();
	}

	void RenderScene::forwardScreenInfo(const ScreenInfo& info) {
		camera->updateDepth(info);
	}





	void Scene::iterAllObjects(function<void(MeshRenderer&)> function) {
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				MeshRenderer& object = *et->second;
				function(object);
			}
		}
	}

	void Scene::iterSceneObjects(std::function<void(SceneObject&)> function) {
		function(*camera);

		iterAllObjects([&](MeshRenderer& object) {
			function(object);
		});

		lightManager.iterLights([&](Light& light) {
			function(light);
		});
	}


	void Scene::iterRenderObjects(function<void(const MeshRenderer&)> function) const {
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				function(object);
			}
		}
	}

	void Scene::iterRenderObjects(SceneShader& shader, function<void(const MeshRenderer&)> function) const {
		auto it = renderObjects.find(&shader);
		if (it != renderObjects.end()) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				function(object);
			}
		}
	}

}