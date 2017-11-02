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
#include "utility/glguards.h"
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
		bool needAdd = false;

		auto it = renderObjects.find(shader.getMethod());
		if (it == renderObjects.end())
			needAdd = true;
		else {
			ShaderMapping& shaders = it->second;
			auto et = shaders.find(&shader);

			needAdd = (et == shaders.end());
		}

		if (needAdd) {
			pipeline.staticBind(*camera, lightManager, shader);
			if (shader.getUseLight()) lightManager.addShaderListener(shader);

			renderObjects[shader.getMethod()][&shader] = TransformMapping();
		}
	}

	void Scene::addMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			//Init shader if it hasn't been added to the scene yet
			addShader(shader);

			renderer.addMaterialChangeListener([this](MeshRenderer& renderer, Material oldMaterial, Material newMaterial) {
				updateMeshRenderer(renderer, oldMaterial, newMaterial);
			});

			renderObjects[shader.getMethod()][&shader][renderer.transform.getID()] = &renderer;
		});
	}



	void Scene::removeMeshRenderer(MeshRenderer& renderer, SceneShader& shader) {
		auto itMethod = renderObjects.find(shader.getMethod());
		if (itMethod != renderObjects.end()) {
			auto& shaders = (*itMethod).second;
			auto itShader = shaders.find(&shader);

			if (itShader != shaders.end()) {
				auto& objects = (*itShader).second;
				auto obj = objects.find(renderer.transform.getID());
				if (obj != objects.end())
					objects.erase(obj);

			}
		}
	}

	void Scene::removeMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			removeMeshRenderer(renderer, shader);
		});
	}


	void Scene::updateMeshRenderer(MeshRenderer& renderer, Material oldMaterial, Material newMaterial) {
		SceneShader& oldShader = oldMaterial.getShader();
		SceneShader& newShader = newMaterial.getShader();

		if (&oldShader == &newShader || oldShader.getMethod() == newShader.getMethod()) return;
		
		//Remove mesh renderer from old materials shaders bucket
		if(!renderer.containsShader(oldShader)) removeMeshRenderer(renderer, oldShader);

		renderObjects[newShader.getMethod()][&newShader][renderer.transform.getID()] = &renderer;
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
	
		addShader(materialFactory.getDefaultShader(ShadingMethod::Generic));
		addShader(materialFactory.getDefaultShader(ShadingMethod::Forward));
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

	
	void RenderScene::draw(ShadingMethod shadingMethod, const Camera& camera) const {
		iterRenderObjects(shadingMethod, [this, &camera](const MeshRenderer& object, SceneShader& shader) {
			pipeline.dynamicBind(lightManager, shader, camera);

			if (object.isActive())
				object.draw(shader);
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

		draw(ShadingMethod::Deferred, camera);
		draw(ShadingMethod::DeferredSkinned, camera);
	}

	void RenderScene::drawDefaultForward(const Camera& camera) const {
		SceneShader& shader = materialFactory.getDefaultShader(ShadingMethod::Generic);

		shader.bind<glm::mat4>("projection", camera.getProjectionMatrix());
		shader.bind<glm::vec3>("cameraPosition", camera.transform.getPosition());

		drawObjects(shader, &camera);
	}


	void RenderScene::drawGeneric(ShadingMethod shadingMethod, const Camera & camera) const {
		SceneShader* currentShader = nullptr;

		iterRenderObjects(shadingMethod, [this, &camera, &currentShader](const MeshRenderer& object, SceneShader& shader) {
			if (currentShader != &shader) {
				shader.bind<glm::mat4>("projection", camera.getProjectionMatrix());
				shader.bind<glm::vec3>("cameraPosition", camera.transform.getPosition());

				pipeline.dynamicBind(lightManager, shader, camera);
				currentShader = &shader;
			}

			if (object.isActive())
				object.draw(shader);
		});
	}

	void RenderScene::drawGeneric() const {
		drawGeneric(*camera);
	}

	void RenderScene::drawGeneric(const Camera& camera) const {
		BlendGuard blend;
		blend.blendAlpha();

		drawGeneric(ShadingMethod::Generic, camera);
		drawGeneric(ShadingMethod::GenericSkinned, camera);
	}


	void RenderScene::drawForward(ShadingMethod shadingMethod, const Camera & camera) const {
		SceneShader* currentShader = nullptr;

		iterRenderObjects(shadingMethod, [this, &camera, &currentShader](const MeshRenderer& object, SceneShader& shader) {
			if (currentShader != &shader) {
				shader.bind<glm::mat4>("projection", camera.getProjectionMatrix());
				shader.bind<glm::mat4>("inverseView", camera.getInverseViewMatrix());
				shader.bind<glm::vec3>("origin", camera.GetOriginInViewSpace());

				pipeline.dynamicBind(lightManager, shader, camera);
				currentShader = &shader;
			}

			if (object.isActive())
				object.draw(shader);
		});

	}

	void RenderScene::drawForward() const {
		drawForward(*camera);
	}

	void RenderScene::drawForward(const Camera & camera) const {
		BlendGuard blend(3);
		blend.blendAlpha();


		drawForward(ShadingMethod::Forward, camera);
		drawForward(ShadingMethod::ForwardSkinned, camera);
	}


	void RenderScene::drawObjects(SceneShader& shader, const Camera* const camera) const {
		if (camera != nullptr)
			pipeline.dynamicBind(lightManager, shader, *camera);
		else
			pipeline.dynamicBind(lightManager, shader);

		iterRenderObjects([&shader](const MeshRenderer& object) {
			if (object.isActive())
				object.drawExclusive(shader);
		});
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

		std::set<MeshRenderer*> renderers;
		for (auto it(renderObjects.begin()); it != renderObjects.end(); it++) {
			ShaderMapping& shaders = it->second;

			for (auto et(shaders.begin()); et != shaders.end(); et++) {
				TransformMapping& elements = et->second;

				for (auto ut(elements.begin()); ut != elements.end(); ut++) {
					MeshRenderer& object = *ut->second;

					//Only call method if it hasn't been called on same renderer before.
					//This check is necessary because same renderer can be in present in 
					//several shader buckets
					if(renderers.find(&object) == renderers.end())
						function(object);

					renderers.emplace(&object);
				}
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
		for (auto it(renderObjects.begin()); it != renderObjects.end(); it++) {
			const ShaderMapping& shaders = it->second;
			
			for (auto et(shaders.begin()); et != shaders.end(); et++) {
				const TransformMapping& elements = et->second;

				for (auto ut(elements.begin()); ut != elements.end(); ut++) {
					const MeshRenderer& object = *ut->second;
					function(object);
				}
			}
		}
	}

	void Scene::iterRenderObjects(SceneShader& shader, function<void(const MeshRenderer&)> function) const {
		auto itMethod = renderObjects.find(shader.getMethod());
		if (itMethod != renderObjects.end()) {
			const ShaderMapping& shaders = itMethod->second;
			auto itShader = shaders.find(&shader);

			if (itShader != shaders.end()) {
				const TransformMapping& elements = itShader->second;

				for (auto ut(elements.begin()); ut != elements.end(); ut++) {
					const MeshRenderer& object = *ut->second;
					function(object);
				}

			}
		}
	}

	void Scene::iterRenderObjects(ShadingMethod shadingMethod, std::function<void(const MeshRenderer&, SceneShader&)> function) const {
		auto itMethod = renderObjects.find(shadingMethod);
		if (itMethod != renderObjects.end()) {
			const ShaderMapping& shaders = itMethod->second;

			for (auto itShader(shaders.begin()); itShader != shaders.end(); itShader++) {
				SceneShader& shader = *itShader->first;
				const TransformMapping& elements = itShader->second;

				for (auto ut(elements.begin()); ut != elements.end(); ut++) {
					const MeshRenderer& object = *ut->second;
					function(object, shader);
				}
			}
		}
	}

}