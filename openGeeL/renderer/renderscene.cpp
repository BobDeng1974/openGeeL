#include <algorithm>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <vector>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "materials/material.h"
#include "meshes/mesh.h"
#include "meshes/model.h"
#include "meshes/meshrenderer.h"
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
			renderers.emplace(&renderer);
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

		renderers.erase(&renderer);
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

		addShader(newShader);
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

	size_t Scene::count(ShadingMethod shadingMethod) const {

		auto itMethod = renderObjects.find(shadingMethod);
		if (itMethod != renderObjects.end()) {
			const ShaderMapping& shaders = itMethod->second;

			if (shaders.size() > 0) {
				size_t count = 0;

				for (auto et(shaders.begin()); et != shaders.end(); et++) {
					const TransformMapping& elements = et->second;

					count += elements.size();

				}

				return count;
			}
		}

		return 0;
	}

	bool Scene::contains(ShadingMethod shadingMethod) const {
		auto itMethod = renderObjects.find(shadingMethod);
		if (itMethod != renderObjects.end()) {
			const ShaderMapping& shaders = itMethod->second;
			return shaders.size() > 0;
		}

		return false;
	}




	RenderScene::RenderScene(Transform& world, LightManager& lightManager, RenderPipeline& pipeline,
		SceneCamera& camera, const MaterialFactory& materialFactory, Input& input)
			: Scene(world, lightManager, pipeline, camera), 
				materialFactory(materialFactory), input(input) {
			
		//Add generic shader to scene to allow method 'drawGenericForced' to work
		addShader(materialFactory.getDefaultShader(ShadingMethod::Generic, false));
		addShader(materialFactory.getDefaultShader(ShadingMethod::Generic, true));
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

	void RenderScene::updateBindings() {
		iterShaders([this](SceneShader& shader) {
			pipeline.dynamicBind(lightManager, shader, *camera);
		});
	}

	
	void RenderScene::draw(ShadingMethod shadingMethod, const Camera& camera, bool updateBinding) const {
		SceneShader* currentShader = nullptr;

		iterRenderObjects(shadingMethod, [this, &camera, &currentShader, &updateBinding]
			(const MeshRenderer& object, SceneShader& shader) {

			if (currentShader != &shader) {
				if(updateBinding) pipeline.dynamicBind(lightManager, shader, camera);
				pipeline.drawingBind(shader);
				currentShader = &shader;
			}

			if (object.isActive()/* && object.isVisible(camera)*/)
				object.draw(shader);
		});
	}

	void RenderScene::drawDefault() const {
		drawDefault(*camera);
	}

	void RenderScene::drawDefault(const Camera& camera) const {
		//Only bind camera if it is external since the scene 
		//camera was already bound during update process
		bool externalCamera = &camera != this->camera;
		if (externalCamera) pipeline.bindCamera(camera);

		draw(ShadingMethod::Deferred, camera, externalCamera);
	}

	void RenderScene::drawGenericForced(const Camera& camera, bool forceGamma) const {
		SceneShader& shader = materialFactory.getDefaultShader(ShadingMethod::Generic);

		if (forceGamma) shader.bind<int>("gammaCorrection", true);

		drawObjects(shader, &camera);

		if (forceGamma) shader.bind<int>("gammaCorrection", false);
	}


	void RenderScene::drawGeneric() const {
		drawGeneric(*camera);
	}

	void RenderScene::drawGeneric(const Camera& camera) const {
		bool externalCamera = &camera != this->camera;

		BlendGuard blend;
		blend.blendAlpha();

		draw(ShadingMethod::Generic, camera, externalCamera);
	}

	void RenderScene::drawForward() const {
		drawForward(*camera);
	}

	void RenderScene::drawForward(const Camera & camera) const {
		bool externalCamera = &camera != this->camera;

		draw(ShadingMethod::Forward, camera, externalCamera);
	}


	void RenderScene::drawForwardOrdered(ShadingMethod shadingMethod, const Camera& camera, bool updateBindings) const {
		using MSPair = pair<const MeshRenderer*, SceneShader*>;

		SceneShader* currentShader = nullptr;

		//Write all objects into sortable data structure
		size_t c = count(shadingMethod);
		vector<MSPair> sortedObjects;
		sortedObjects.reserve(c);
		iterRenderObjects(shadingMethod, [&sortedObjects](const MeshRenderer& object, SceneShader& shader) {
			sortedObjects.push_back(MSPair(&object, &shader));
		});

		//Sort by distance to camera
		sort(sortedObjects.begin(), sortedObjects.end(), [&camera](const MSPair& a, const MSPair& b) -> bool {
			const MeshRenderer& x = *a.first;
			const MeshRenderer& y = *b.first;

			glm::vec3 camPos = camera.transform.getPosition();
			float xDist = glm::distance(x.transform.getPosition(), camPos);
			float yDist = glm::distance(y.transform.getPosition(), camPos);

			return xDist > yDist;
		});

		//Read sorted data structure
		for (auto it(sortedObjects.begin()); it != sortedObjects.end(); it++) {
			const MeshRenderer& object = *it->first;
			SceneShader& shader = *it->second;

			if (currentShader != &shader) {
				if(updateBindings) pipeline.dynamicBind(lightManager, shader, camera);
				pipeline.drawingBind(shader);
				currentShader = &shader;
			}

			if (object.isActive()/* && object.isVisible(camera)*/)
				object.draw(shader);
		}

	}

	void RenderScene::drawTransparentOD() const {
		drawTransparentOD(*camera);
	}

	void RenderScene::drawTransparentOD(const Camera& camera) const {
		drawForwardOrdered(ShadingMethod::TransparentOD, camera);
	}


	void RenderScene::drawTransparentOID() const {
		drawTransparentOID(*camera);
	}

	void RenderScene::drawTransparentOID(const Camera& camera) const {
		bool externalCamera = &camera != this->camera;

		draw(ShadingMethod::TransparentOID, camera, externalCamera);
	}


	void RenderScene::drawObjects(SceneShader& shader, const Camera* const camera) const {
		if (camera != nullptr)
			pipeline.dynamicBind(lightManager, shader, *camera);
		else
			pipeline.dynamicBind(lightManager, shader);

		pipeline.drawingBind(shader);

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

	

	void Scene::iterSceneObjects(std::function<void(SceneObject&)> function) {
		function(*camera);

		iterRenderObjects([&](MeshRenderer& object) {
			function(object);
		});

		lightManager.iterLights([&](Light& light) {
			function(light);
		});
	}


	void Scene::iterRenderObjects(function<void(MeshRenderer&)> function) const {
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			MeshRenderer& renderer = **it;
			function(renderer);
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

	void Scene::iterShaders(std::function<void(SceneShader&)> function) {
		for (auto itMethod(renderObjects.begin()); itMethod != renderObjects.end(); itMethod++) {
			const ShaderMapping& shaders = itMethod->second;
			
			for (auto itShader(shaders.begin()); itShader != shaders.end(); itShader++) {
				SceneShader& shader = *itShader->first;
				function(shader);
			}
		}
	}

	
}