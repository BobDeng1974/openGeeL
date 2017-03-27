#include <algorithm>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include "shader\shader.h"
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
#include "utility\gbuffer.h"
#include "physics\physics.h"
#include "lighting\lightmanager.h"
#include "shader\shadermanager.h"
#include "transformation\transform.h"
#include "scene.h"
#include <iostream>

using namespace std;

namespace geeL {

	RenderScene::RenderScene(Transform& world, LightManager& lightManager, ShaderInformationLinker& shaderManager, 
		Camera& camera, MeshFactory& meshFactory, MaterialFactory& materialFactory)
			: lightManager(lightManager), shaderLinker(shaderManager), camera(&camera), meshFactory(meshFactory),
				physics(nullptr), worldTransform(world), materialFactory(materialFactory) {}

	

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

		originViewSpace = TranslateToViewSpace(glm::vec3(0.f, 0.f, 0.f));
		shaderLinker.bindCamera(*this);

		//TODO: Move this to a seperate thread to allow simulation at fixed frame rate
		if (physics != nullptr)
			physics->update();
	}

	void RenderScene::draw(SceneShader& shader) {
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

		shaderLinker.dynamicBind(*this, materialFactory.getDeferredShader());
		iterRenderObjects(materialFactory.getDeferredShader(), [&](const MeshRenderer& object) {
			if (object.isActive())
				object.draw(materialFactory.getDeferredShader());
		});

		iterSkinnedObjects(materialFactory.getDeferredShader(), [&](const SkinnedMeshRenderer& object) {
			if (object.isActive())
				object.draw();
		});
	}

	void RenderScene::drawForward() const {

		//Draw all registered objects with shaders other than deferred shader
		const SceneShader& defShader = materialFactory.getDeferredShader();
		for (auto it = renderObjects.begin(); it != renderObjects.end(); it++) {
			SceneShader* shader = it->first;
			if (shader == &defShader)
				continue;

			shaderLinker.dynamicBind(*this, *shader);

			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MeshRenderer& object = *et->second;
				if (object.isActive())
					object.draw();
			}
		}

		const SceneShader& defSkinnedShader = materialFactory.getDefaultShader(DefaultShading::DeferredSkinned);
		for (auto it = skinnedObjects.begin(); it != skinnedObjects.end(); it++) {
			const SceneShader* shader = it->first;
			if (shader == &defSkinnedShader)
				continue;

			auto& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const SkinnedMeshRenderer& object = *et->second;
				if (object.isActive())
					object.draw();
			}
		}
	}
	
	void RenderScene::drawObjects(const Shader& shader) const {
		drawStaticObjects(shader);
		drawSkinnedObjects(shader);
	}

	void RenderScene::drawStaticObjects(const Shader& shader) const {
		shader.use();

		iterRenderObjects([&](const MeshRenderer& object) {
			if (object.isActive())
				object.drawExclusive(shader);
		});
	}

	void RenderScene::drawSkinnedObjects(const Shader& shader) const {
		shader.use();

		iterSkinnedObjects([&](const MeshRenderer& object) {
			if (object.isActive())
				object.drawExclusive(shader);
		});
	}

	void RenderScene::setSkybox(Skybox& skybox) {
		this->skybox = &skybox;
	}

	void RenderScene::drawSkybox() const {
		if (skybox != nullptr)
			skybox->draw(*camera);
	}

	void RenderScene::bindSkybox(Shader& shader) const {
		if (skybox != nullptr)
			skybox->bind(shader);
	}

	const Camera& RenderScene::getCamera() const {
		return *camera;
	}

	Camera& RenderScene::getCamera() {
		return *camera;
	}


	void RenderScene::AddMeshRenderer(MeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			//Init shader if it hadn't been added to the scene before
			auto it = renderObjects.find(&shader);
			if (it == renderObjects.end())
				shaderLinker.staticBind(*this, shader);

			renderObjects[&shader][renderer.transform.getID()] = &renderer;
		});

	}

	void RenderScene::AddSkinnedMeshRenderer(SkinnedMeshRenderer& renderer) {
		renderer.iterateShaders([this, &renderer](SceneShader& shader) {
			//Init shader if it hadn't been added to the scene before
			auto it = skinnedObjects.find(&shader);
			if (it == skinnedObjects.end())
				shaderLinker.staticBind(*this, shader);

			skinnedObjects[&shader][renderer.transform.getID()] = &renderer;
		});
	}


	void RenderScene::forwardScreenInfo(const ScreenInfo& info) {
		camera->updateDepth(info);
	}

	glm::vec3 RenderScene::TranslateToScreenSpace(const glm::vec3& vector) const {
		glm::vec4 vec = camera->getProjectionMatrix() * camera->getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x / vec.w, vec.y / vec.w, vec.z / vec.w) * 0.5f + 0.5f;
	}

	glm::vec3 RenderScene::TranslateToViewSpace(const glm::vec3& vector) const {
		glm::vec4 vec = camera->getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 RenderScene::TranslateToWorldSpace(const glm::vec3& vector) const {
		glm::vec4 vec = camera->getInverseViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	const glm::vec3& RenderScene::GetOriginInViewSpace() const {
		return originViewSpace;
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