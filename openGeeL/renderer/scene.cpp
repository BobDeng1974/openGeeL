#include <algorithm>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include "shader\shader.h"
#include "materials\material.h"
#include "meshes\mesh.h"
#include "meshes\model.h"
#include "meshes\meshrenderer.h"
#include "meshes\skinnedrenderer.h"
#include "meshes\meshfactory.h"
#include "cameras\camera.h"
#include "cubemapping\skybox.h"
#include "utility\gbuffer.h"
#include "physics\physics.h"
#include "lighting\lightmanager.h"
#include "transformation\transform.h"
#include "scene.h"

using namespace std;

namespace geeL {

	RenderScene::RenderScene(LightManager& lightManager, Camera& camera, MeshFactory& meshFactory, Transform& world)
		: lightManager(lightManager), camera(camera), meshFactory(meshFactory), physics(nullptr), worldTransform(world) {}

	

	void RenderScene::update() {

		camera.update();
		iterRenderObjects([&](MeshRenderer* object) {
			object->update();
		});

		worldTransform.update();

		camera.lateUpdate();
		iterRenderObjects([&](MeshRenderer* object) {
			object->lateUpdate();
		});

		originViewSpace = TranslateToViewSpace(glm::vec3(0.f, 0.f, 0.f));

		//TODO: Move this to a seperate thread to allow simulation at fixed frame rate
		if (physics != nullptr)
			physics->update();
	}

	void RenderScene::drawDeferred() const {
		iterRenderObjects(RenderObjectsMode::Deferred, [&](const MeshRenderer* object) {
			if (object->isActive())
				object->draw(true);
		});
	}

	void RenderScene::drawForward() const {
		iterRenderObjects(RenderObjectsMode::Forward, [&](const MeshRenderer* object) {
			if (object->isActive())
				object->draw(false);
		});
	}
	
	void RenderScene::drawObjects(const Shader& shader) const {
		shader.use();

		iterRenderObjects([&](const MeshRenderer* object) {
			if (object->isActive())
				object->draw(shader);
		});
	}

	void RenderScene::drawStaticObjects(const Shader& shader) const {
		shader.use();

		iterRenderObjects(RenderObjectsMode::Static, [&](const MeshRenderer* object) {
			if (object->isActive())
				object->draw(shader);
		});
	}

	void RenderScene::drawSkinnedObjects(const Shader& shader) const {
		shader.use();

		iterRenderObjects(RenderObjectsMode::Skinned, [&](const MeshRenderer* object) {
			if (object->isActive())
				object->draw(shader);
		});
	}

	void RenderScene::setSkybox(Skybox& skybox) {
		this->skybox = &skybox;
	}

	void RenderScene::drawSkybox() const {
		if (skybox != nullptr)
			skybox->draw(camera);
	}

	void RenderScene::bindSkybox(Shader& shader) const {
		if (skybox != nullptr)
			skybox->bind(shader);
	}


	void RenderScene::AddMeshRenderer(MeshRenderer& renderer) {
		deferredRenderObjects.push_back(&renderer);
	}

	void RenderScene::AddSkinnedMeshRenderer(SkinnedMeshRenderer& renderer) {
		deferredSkinnedObjects.push_back(&renderer);
	}


	void RenderScene::forwardScreenInfo(const ScreenInfo& info) {
		camera.updateDepth(info);
		lightManager.forwardScreenInfo(info, camera);
	}

	glm::vec3 RenderScene::TranslateToScreenSpace(const glm::vec3& vector) const {
		glm::vec4 vec = camera.getProjectionMatrix() * camera.getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x / vec.w, vec.y / vec.w, vec.z / vec.w) * 0.5f + 0.5f;
	}

	glm::vec3 RenderScene::TranslateToViewSpace(const glm::vec3& vector) const {
		glm::vec4 vec = camera.getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 RenderScene::TranslateToWorldSpace(const glm::vec3& vector) const {
		glm::vec4 vec = camera.getInverseViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	const glm::vec3& RenderScene::GetOriginInViewSpace() const {
		return originViewSpace;
	}

	unsigned int RenderScene::getSkyboxID() const {
		if (skybox != nullptr)
			return skybox->getID();

		return 0;
	}

	void RenderScene::setPhysics(Physics* physics) {
		this->physics = physics;
	}



	void RenderScene::iterRenderObjects(RenderObjectsMode mode, std::function<void(MeshRenderer*)> function) {
		switch (mode) {
			case RenderObjectsMode::Deferred:
				for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);

				for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::DeferredStatic:
				for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
				break;
			case RenderObjectsMode::DeferredSkinned:
				for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::Static:
				for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
				for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
				break;
			case RenderObjectsMode::Skinned:
				for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
				for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::Forward:
				for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);

				for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::ForwardStatic:
				for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
				break;
			case RenderObjectsMode::ForwardSkinned:
				for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
		}
	}

	void RenderScene::iterRenderObjects(RenderObjectsMode mode, std::function<void(const MeshRenderer*)> function) const {
		switch (mode) {
			case RenderObjectsMode::Deferred:
				for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);

				for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::DeferredStatic:
				for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
				break;
			case RenderObjectsMode::DeferredSkinned:
				for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::Static:
				for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
				for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
				break;
			case RenderObjectsMode::Skinned:
				for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
				for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::Forward:
				for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);

				for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
			case RenderObjectsMode::ForwardStatic:
				for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
				for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
				break;
			case RenderObjectsMode::ForwardSkinned:
				for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
				for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
				break;
		}
	}

	void RenderScene::iterRenderObjects(std::function<void(MeshRenderer*)> function) {
		for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
		for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);

		for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
		for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
		for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
	}

	void RenderScene::iterRenderObjects(std::function<void(const MeshRenderer*)> function) const {
		for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
		for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);

		for_each(deferredSkinnedObjects.begin(), deferredSkinnedObjects.end(), function);
		for_each(forwardSkinnedObjects.begin(), forwardSkinnedObjects.end(), function);
		for_each(mixedSkinnedObjects.begin(), mixedSkinnedObjects.end(), function);
	}

}