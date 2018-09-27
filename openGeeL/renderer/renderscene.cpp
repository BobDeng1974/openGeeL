#include <algorithm>
#include <vec3.hpp>
#include <vector>
#include "shader/sceneshader.h"
#include "shader/uniformstack.h"
#include "meshes/meshrenderer.h"
#include "materials/materialfactory.h"
#include "cameras/camera.h"
#include "cubemapping/skybox.h"
#include "lights/light.h"
#include "lights/lightmanager.h"
#include "inputmanager.h"
#include "transformation/transform.h"
#include "utility/worldinformation.h"
#include "glwrapper/glguards.h"
#include "renderscene.h"

using namespace std;


namespace geeL {

	Scene::Scene(Transform& world, 
		LightManager& lightManager, 
		UniformBindingStack& pipeline, 
		SceneCamera& camera)
			: lightManager(lightManager)
			, pipeline(pipeline)
			, camera(&camera)
			, worldTransform(world)
			, skybox(nullptr) {
	
		addRequester(lightManager);
	}

	Scene::~Scene() {
		for (auto it(renderers.begin()); it != renderers.end(); it++) {
			MeshRenderer* renderer = it->second;
			onRemove(std::shared_ptr<MeshRenderer>(renderer));
		}
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
			pipeline.staticBind(*camera, shader);
			if (shader.getUseLight()) lightManager.addShaderListener(shader);

			renderObjects[shader.getMethod()][&shader] = TransformMapping();
		}
	}

	MeshRenderer& Scene::addMeshRenderer(std::unique_ptr<MeshRenderer> renderer) {
		MeshRenderer* rawRenderer = renderer.release();

		renderers[rawRenderer->getID()] = rawRenderer;
		onAdd(*rawRenderer);

		SceneShader& shader = rawRenderer->getShader();
		addShader(shader);

		rawRenderer->addMaterialChangeListener([this](MeshRenderer& renderer, Material oldMaterial, Material newMaterial) {
			updateMeshRenderer(renderer, oldMaterial, newMaterial);
		});

		renderObjects[shader.getMethod()][&shader][rawRenderer->getID()] = rawRenderer;

		return *rawRenderer;
	}



	void Scene::removeMeshRenderer(MeshRenderer& renderer, SceneShader& shader) {
		auto itMethod = renderObjects.find(shader.getMethod());
		if (itMethod != renderObjects.end()) {
			auto& shaders = (*itMethod).second;
			auto itShader = shaders.find(&shader);

			if (itShader != shaders.end()) {
				auto& objects = (*itShader).second;
				auto obj = objects.find(renderer.getID());
				if (obj != objects.end()) {
					objects.erase(obj);
				}
			}
		}
	}

	void Scene::removeMeshRenderer(MeshRenderer& renderer) {
		auto toRemove = renderers.find(renderer.getID());

		if (toRemove != renderers.end()) {
			removeMeshRenderer(renderer, renderer.getShader());
			renderers.erase(toRemove);
			onRemove(std::shared_ptr<MeshRenderer>(&renderer));
		}
	}


	void Scene::updateMeshRenderer(MeshRenderer& renderer, Material oldMaterial, Material newMaterial) {
		SceneShader& oldShader = oldMaterial.getShader();
		SceneShader& newShader = newMaterial.getShader();

		if (&oldShader == &newShader || oldShader.getMethod() == newShader.getMethod()) return;

		//Remove mesh renderer from old materials shaders bucket
		removeMeshRenderer(renderer, oldShader);

		addShader(newShader);
		renderObjects[newShader.getMethod()][&newShader][renderer.getID()] = &renderer;
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

	bool Scene::containsStaticObjects() const {
		bool value = false;
		iterShaders([&value](const SceneShader& shader) -> bool {
			if (!shader.isAnimated()) {
				value = true;
				return true;
			}

			return false;
		});

		return value;
	}

	bool Scene::containsSkinnedObjects() const {
		bool value = false;
		iterShaders([&value](const SceneShader& shader) -> bool {
			if (shader.isAnimated()) {
				value = true;
				return true;
			}

			return false;
		});

		return value;
	}



	RenderScene::RenderScene(Transform& world, 
		LightManager& lightManager, 
		UniformBindingStack& pipeline,
		SceneCamera& camera, 
		MaterialFactory& materialFactory, 
		Input& input)
			: Scene(world, lightManager, pipeline, camera)
			, materialFactory(materialFactory)
			, input(input) {
			
		//Add generic shader to scene to allow method 'drawGenericForced' to work
		addShader(materialFactory.getDefaultShader(ShadingMethod::Forward, false));
		addShader(materialFactory.getDefaultShader(ShadingMethod::Forward, true));
	}

	MeshRenderer& RenderScene::addMeshRenderer(std::unique_ptr<MeshRenderer> renderer) {
		std::lock_guard<std::mutex> lock(mutex);

		return Scene::addMeshRenderer(std::move(renderer));
	}

	void RenderScene::removeMeshRenderer(MeshRenderer& renderer) {
		std::lock_guard<std::mutex> lock(mutex);

		Scene::removeMeshRenderer(renderer);
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

		updateUpdateListener();
	}

	void RenderScene::updateBindings() {
		iterShaders([this](SceneShader& shader) {
			shader.loadSceneInformation(lightManager, *camera);
		});
	}



	void RenderScene::draw(ShadingMethod method) {
		draw(method, *camera);
	}

	void RenderScene::draw(ShadingMethod method, const Camera& camera) {
		switch (method) {
			case ShadingMethod::Deferred:
				drawDefault(camera);
				break;
			case ShadingMethod::Forward:
				drawForward(camera);
				break;
			case ShadingMethod::Hybrid:
				drawHybrid(camera);
				break;
			case ShadingMethod::Transparent:
				drawTransparent(camera);
				break;
		}
	}

	
	void RenderScene::draw(ShadingMethod shadingMethod, const Camera& camera, bool updateBinding) {
		SceneShader* currentShader = nullptr;

		iterRenderObjects(shadingMethod, [this, &camera, &currentShader, &updateBinding]
			(MeshRenderer& object, SceneShader& shader) {

			if (currentShader != &shader) {
				if (currentShader != nullptr) currentShader->forceDraw();
				if(updateBinding) shader.loadSceneInformation(lightManager, camera);
				currentShader = &shader;
			}

			if (object.isActive() && object.isVisible(camera))
				object.draw();
		});

		if (currentShader != nullptr) currentShader->forceDraw();
	}


	void RenderScene::drawDefault(const Camera& camera) {
		//Only bind camera if it is external since the scene 
		//camera was already bound during update process
		bool externalCamera = &camera != this->camera;
		if (externalCamera) pipeline.bindCamera(camera);

		draw(ShadingMethod::Deferred, camera, externalCamera);
	}

	void RenderScene::drawForwardForced(const Camera& camera, bool forceGamma) {
		SceneShader& shader = materialFactory.getDefaultShader(ShadingMethod::Forward, false);

		if (forceGamma) shader.bind<int>("gammaCorrection", true);

		drawObjects(shader, &camera);

		if (forceGamma) shader.bind<int>("gammaCorrection", false);
	}

	void RenderScene::drawForward(const Camera& camera) {
		bool externalCamera = &camera != this->camera;

		BlendGuard blend;
		blend.blendAlpha();

		draw(ShadingMethod::Forward, camera, externalCamera);
	}


	void RenderScene::drawHybrid(const Camera& camera) {
		bool externalCamera = &camera != this->camera;

		draw(ShadingMethod::Hybrid, camera, externalCamera);
	}

	void RenderScene::drawForwardOrdered(ShadingMethod shadingMethod, const Camera& camera, bool updateBindings) {
		using MSPair = pair<MeshRenderer*, SceneShader*>;
		SceneShader* currentShader = nullptr;

		//Write all objects into sortable data structure
		size_t c = count(shadingMethod);
		vector<MSPair> sortedObjects;
		sortedObjects.reserve(c);
		iterRenderObjects(shadingMethod, [&sortedObjects](MeshRenderer& object, SceneShader& shader) {
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
			MeshRenderer& object = *it->first;
			SceneShader& shader = *it->second;

			if (currentShader != &shader) {
				if (currentShader != nullptr) currentShader->forceDraw();
				if(updateBindings) shader.loadSceneInformation(lightManager, camera);
				currentShader = &shader;
			}

			if (object.isActive() && object.isVisible(camera))
				object.draw();
		}

		if (currentShader != nullptr) currentShader->forceDraw();
	}

	void RenderScene::updateUpdateListener() {
		for (auto it(updateListeners.begin()); it != updateListeners.end(); it++)
			(*it)(*this);
	}

	void RenderScene::drawTransparent(const Camera& camera) {
		drawForwardOrdered(ShadingMethod::Transparent, camera);
	}


	void RenderScene::drawObjects(SceneShader& shader, const Camera* const camera) {
		shader.loadSceneInformation(lightManager, camera);

		iterRenderObjects([&shader, &camera](MeshRenderer& object) {
			if (object.isActive()) {
				//Use frustum culling if a camera has been attached
				bool isVisible = !((camera != nullptr) && !object.isVisible(*camera));

				if (isVisible)
					object.drawExclusive(shader);
			}
		});

		shader.forceDraw();
	}
	
	void RenderScene::drawGeometry(const RenderShader& shader) {
		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive())
				object.drawGeometry(shader);
		});
	}

	void RenderScene::drawGeometry(const RenderShader& shader, RenderMode mode, const ViewFrustum* const frustum) const {
		iterRenderObjects([&](MeshRenderer& object) {
			if (object.isActive()) {
				//Use frustum culling if frustum has been attached
				bool isVisible = !((frustum != nullptr) && !object.isVisible(*frustum));

				if (object.getRenderMode() == mode && isVisible)
					object.drawGeometry(shader);
			}
		});
	}

	void RenderScene::drawGeometry(const RenderShader& staticShader, const RenderShader& skinnedShader, 
		const ViewFrustum* const frustum) const {

		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive()) {
				//Use frustum culling if frustum has been attached
				bool isVisible = !((frustum != nullptr) && !object.isVisible(*frustum));

				if (isVisible) {
					const RenderShader& s = (object.getRenderMode() == RenderMode::Static)
						? staticShader : skinnedShader;

					object.drawGeometry(s);
				}
			}
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

	void RenderScene::addUpdateListener(std::function<void(RenderScene&)> listener) {
		updateListeners.push_back(listener);
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
			MeshRenderer& renderer = *it->second;
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

	void Scene::iterRenderObjects(ShadingMethod shadingMethod, std::function<void(MeshRenderer&, SceneShader&)> function) {
		auto itMethod = renderObjects.find(shadingMethod);
		if (itMethod != renderObjects.end()) {
			const ShaderMapping& shaders = itMethod->second;

			for (auto itShader(shaders.begin()); itShader != shaders.end(); itShader++) {
				SceneShader& shader = *itShader->first;
				const TransformMapping& elements = itShader->second;

				for (auto ut(elements.begin()); ut != elements.end(); ut++) {
					MeshRenderer& object = *ut->second;
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

	void Scene::iterShaders(std::function<bool(const SceneShader&)> function) const {
		for (auto itMethod(renderObjects.begin()); itMethod != renderObjects.end(); itMethod++) {
			const ShaderMapping& shaders = itMethod->second;

			for (auto itShader(shaders.begin()); itShader != shaders.end(); itShader++) {
				const SceneShader& shader = *itShader->first;
				
				if (function(shader)) return;
			}
		}
	}

}