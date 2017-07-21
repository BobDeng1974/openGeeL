#include <algorithm>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include "shader\rendershader.h"
#include "shader\sceneshader.h"
#include "materials\material.h"
#include "meshes\mesh.h"
#include "meshes\model.h"
#include "meshes\meshrenderer.h"
#include "meshes\skinnedrenderer.h"
#include "meshes\meshfactory.h"
#include "materials\materialfactory.h"
#include "cameras\camera.h"
#include "cubemapping\skybox.h"
#include "utility\screeninfo.h"
#include "physics\physics.h"
#include "lights\lightmanager.h"
#include "pipeline.h"
#include "transformation\transform.h"
#include "utility\worldinformation.h"
#include "scene.h"

using namespace std;

namespace geeL {

	RenderScene::RenderScene(Transform& world, LightManager& lightManager, RenderPipeline& shaderManager, 
		SceneCamera& camera, MaterialFactory& materialFactory)
			: lightManager(lightManager), pipeline(shaderManager), camera(&camera),
				physics(nullptr), worldTransform(world), materialFactory(materialFactory), skybox(nullptr) {}

	
	void RenderScene::init() {
		lightManager.drawShadowmapsForced(*this, camera);
	}

	void RenderScene::updateProbes() {
		lightManager.drawReflectionProbes();
	}

	void RenderScene::update() {
		camera->update();
		iterAllObjects([&](MeshRenderer& object) {
			object.update();
		});

		worldTransform.update();

		camera->lateUpdate();
		iterAllObjects([&](MeshRenderer& object) {
			object.lateUpdate();
		});

		pipeline.bindCamera(*camera);

		//TODO: Move this to a seperate thread to allow simulation at fixed frame rate
		if (physics != nullptr)
			physics->update();
	}

	void RenderScene::draw(SceneShader& shader) {
		pipeline.dynamicBind(*camera, lightManager, shader);

		//Try to find static object with shader first and draw if successfull
		bool found = iterRenderObjects(shader, [&](const MeshRenderer& object) {
			if (object.isActive())
				object.draw();
		});

		//Otherwise try to draw skinned objects
		if (!found) {
			iterSkinnedObjects(shader, [&](const SkinnedMeshRenderer& object) {
				if (object.isActive())
					object.draw();
			});
		}
	}

	void RenderScene::drawDeferred() const {
		drawDeferred(*camera);
	}

	void RenderScene::drawDeferred(const Camera& camera) const {

		//Only bind camera if it is external since the scene 
		//camera was already bound during update process 
		if (&camera != this->camera)
			pipeline.bindCamera(camera);

		SceneShader* shader = &materialFactory.getDeferredShader();
		pipeline.dynamicBind(camera, lightManager, *shader);
		iterRenderObjects(*shader, [&](const MeshRenderer& object) {
			if (object.isActive())
				object.draw(materialFactory.getDeferredShader());
		});

		shader = &materialFactory.getDefaultShader(DefaultShading::DeferredSkinned);
		pipeline.dynamicBind(camera, lightManager, *shader);
		iterSkinnedObjects(*shader, [&](const SkinnedMeshRenderer& object) {
			if (object.isActive())
				object.draw();
		});
	}

	void RenderScene::drawForward() const {
		drawForward(*camera);
	}

	void RenderScene::drawForward(const Camera& camera) const {

		//Only bind camera if it is external since the scene 
		//camera was already bound during update process 
		if (&camera != this->camera)
			pipeline.bindCamera(camera);

		//Draw all registered objects with shaders other than deferred shader
		const SceneShader& defShader = materialFactory.getDeferredShader();
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			SceneShader* shader = it->first;
			if (shader == &defShader)
				continue;

			pipeline.dynamicBind(camera, lightManager, *shader);

			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				if (object.isActive())
					object.draw();
			}
		}

		const SceneShader& defSkinnedShader = materialFactory.getDefaultShader(DefaultShading::DeferredSkinned);
		for (auto it = skinnedObjects.begin(); it != skinnedObjects.end(); it++) {
			SceneShader* shader = it->first;
			if (shader == &defSkinnedShader)
				continue;

			pipeline.dynamicBind(camera, lightManager, *shader);

			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const SkinnedMeshRenderer& object = *et->second;
				if (object.isActive())
					object.draw();
			}
		}
	}

	void RenderScene::drawObjects(SceneShader& shader) const {
		pipeline.dynamicBind(*camera, lightManager, shader);

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
		drawStaticObjects(shader);
		drawSkinnedObjects(shader);
	}

	void RenderScene::drawStaticObjects(const RenderShader& shader) const {
		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive())
				object.drawGeometry(shader);
		});
	}

	void RenderScene::drawSkinnedObjects(const RenderShader& shader) const {
		iterSkinnedObjects([&](const MeshRenderer& object) {
			if (object.isActive())
				object.drawGeometry(shader);
		});
	}

	void RenderScene::setSkybox(Skybox& skybox) {
		this->skybox = &skybox;

		for (auto it = sceneRequester.begin(); it != sceneRequester.end(); it++) {
			SceneRequester& requester = **it;
			requester.updateSkybox(*this->skybox);
		}

		//Redraw reflection probes since skybox is also visible in them
		lightManager.drawReflectionProbes();
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

	void RenderScene::addRequester(SceneRequester& requester) {
		sceneRequester.push_back(&requester);

		requester.updateSkybox(*skybox);
		requester.updateCamera(*camera);
	}

	void RenderScene::setCamera(SceneCamera& camera) {
		this->camera = &camera;

		for (auto it = sceneRequester.begin(); it != sceneRequester.end(); it++) {
			SceneRequester& requester = **it;
			requester.updateCamera(*this->camera);
		}
	}

	const SceneCamera& RenderScene::getCamera() const {
		return *camera;
	}

	SceneCamera& RenderScene::getCamera() {
		return *camera;
	}

	void RenderScene::addShader(SceneShader& shader) {
		auto it = renderObjects.find(&shader);
		if (it == renderObjects.end()) {
			pipeline.staticBind(*camera, lightManager, shader);
			lightManager.addShaderListener(shader);
		}
	}

	void RenderScene::addMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			//Init shader if it hasn't been added to the scene yet
			addShader(shader);

			renderObjects[&shader][renderer.transform.getID()] = &renderer;
		});
	}

	void RenderScene::addMeshRenderer(SkinnedMeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			//Init shader if it hasn't been added to the scene before
			addShader(shader);

			skinnedObjects[&shader][renderer.transform.getID()] = &renderer;
		});
	}

	void RenderScene::removeMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			auto shaders = renderObjects.find(&shader);
			if (shaders != renderObjects.end()) {
				auto& objects = (*shaders).second;

				auto obj = objects.find(renderer.transform.getID());
				if (obj != objects.end())
					objects.erase(obj);
			}

			//TODO: maybe also remove shader when this renderer was the last associated object
		});

	}

	void RenderScene::removeMeshRenderer(SkinnedMeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			auto shaders = skinnedObjects.find(&shader);
			if (shaders != skinnedObjects.end()) {
				auto& objects = (*shaders).second;

				auto obj = objects.find(renderer.transform.getID());
				if (obj != objects.end())
					objects.erase(obj);
			}
		});
	}


	void RenderScene::forwardScreenInfo(const ScreenInfo& info) {
		camera->updateDepth(info);
	}

	void RenderScene::setPhysics(Physics* physics) {
		this->physics = physics;
	}


	void RenderScene::iterAllObjects(function<void(MeshRenderer&)> function) {
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				MeshRenderer& object = *et->second;
				function(object);
			}
		}

		for (auto it = skinnedObjects.begin(); it != skinnedObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				SkinnedMeshRenderer& object = *et->second;
				function(object);
			}
		}
	}

	void RenderScene::iterRenderObjects(function<void(const MeshRenderer&)> function) const {
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				function(object);
			}
		}
	}

	bool RenderScene::iterRenderObjects(SceneShader& shader, function<void(const MeshRenderer&)> function) const {
		auto it = renderObjects.find(&shader);
		if (it != renderObjects.end()) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				function(object);
			}

			return true;
		}

		return false;
	}

	void RenderScene::iterSkinnedObjects(function<void(const MeshRenderer&)> function) const {
		for (auto it = skinnedObjects.begin(); it != skinnedObjects.end(); it++) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const SkinnedMeshRenderer& object = *et->second;
				function(object);
			}
		}
	}

	bool RenderScene::iterSkinnedObjects(SceneShader& shader, function<void(const SkinnedMeshRenderer&)> function) const {
		auto it = skinnedObjects.find(&shader);
		if (it != skinnedObjects.end()) {
			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const SkinnedMeshRenderer& object = *et->second;
				function(object);
			}

			return true;
		}

		return false;
	}


}