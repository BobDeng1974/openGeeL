#include <algorithm>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include "shader\shader.h"
#include "materials\material.h"
#include "meshes\mesh.h"
#include "meshes\model.h"
#include "meshes\meshrenderer.h"
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

	
	RenderScene::~RenderScene() {
		iterRenderObjects(RenderMode::All, [&](MeshRenderer* object) {
			delete object;
		});
	}


	void RenderScene::update() {

		camera.update();
		iterRenderObjects(RenderMode::All, [&](MeshRenderer* object) {
			object->update();
		});

		worldTransform.update();

		camera.lateUpdate();
		iterRenderObjects(RenderMode::All, [&](MeshRenderer* object) {
			object->lateUpdate();
		});

		originViewSpace = TranslateToViewSpace(glm::vec3(0.f, 0.f, 0.f));

		//TODO: Move this to a seperate thread to allow simulation at fixed frame rate
		if (physics != nullptr)
			physics->update();
	}

	void RenderScene::drawDeferred() const {
		iterRenderObjects(RenderMode::Deferred, [&](MeshRenderer* object) {
			if (object->isActive())
				object->draw(true);
		});
	}

	void RenderScene::drawForward() const {
		iterRenderObjects(RenderMode::Forward, [&](MeshRenderer* object) {
			if (object->isActive())
				object->draw(false);
		});
	}
	
	void RenderScene::drawObjects(const Shader& shader) const {
		shader.use();

		iterRenderObjects(RenderMode::All, [&](MeshRenderer* object) {
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


	MeshRenderer& RenderScene::AddMeshRenderer(string modelPath, Transform& transform, 
		CullingMode faceCulling, bool deferred, string name) {
		
		StaticModel& model = meshFactory.CreateStaticModel(modelPath);
		if(deferred)
			deferredRenderObjects.push_back(meshFactory.CreateMeshRendererManual(model, transform, faceCulling, deferred, name));
		else
			forwardRenderObjects.push_back(meshFactory.CreateMeshRendererManual(model, transform, faceCulling, deferred, name));

		return *deferredRenderObjects.back();
	}

	MeshRenderer& RenderScene::AddMeshRenderer(std::string modelPath, Transform& transform,
		std::vector<Material*> materials, CullingMode faceCulling, string name) {

		StaticModel& model = meshFactory.CreateStaticModel(modelPath);
		MeshRenderer* renderer = meshFactory.CreateMeshRendererManual(model, transform, faceCulling, true, name);
		renderer->customizeMaterials(materials);

		bool containsDeferred = renderer->containsDeferredMaterials();
		bool containsForward = renderer->containsForwardMaterials();

		if (containsDeferred && containsForward)
			mixedRenderObjects.push_back(renderer);
		if (containsDeferred)
			deferredRenderObjects.push_back(renderer);
		if (containsForward)
			forwardRenderObjects.push_back(renderer);

		return *renderer;
	}

	MeshRenderer& RenderScene::AddSkinnedMeshRenderer(string modelPath, Transform& transform,
		CullingMode faceCulling, bool deferred, string name) {

		SkinnedModel& model = meshFactory.CreateSkinnedModel(modelPath);
		if (deferred)
			deferredRenderObjects.push_back(meshFactory.CreateSkinnedMeshRendererManual(model, transform, faceCulling, deferred, name));
		else
			forwardRenderObjects.push_back(meshFactory.CreateSkinnedMeshRendererManual(model, transform, faceCulling, deferred, name));

		return *deferredRenderObjects.back();
	}

	MeshRenderer& RenderScene::AddSkinnedMeshRenderer(std::string modelPath, Transform& transform,
		std::vector<Material*> materials, CullingMode faceCulling, string name) {

		SkinnedModel& model = meshFactory.CreateSkinnedModel(modelPath);
		MeshRenderer* renderer = meshFactory.CreateSkinnedMeshRendererManual(model, transform, faceCulling, true, name);
		renderer->customizeMaterials(materials);

		bool containsDeferred = renderer->containsDeferredMaterials();
		bool containsForward  = renderer->containsForwardMaterials();

		if (containsDeferred && containsForward)
			mixedRenderObjects.push_back(renderer);
		if (containsDeferred)
			deferredRenderObjects.push_back(renderer);
		if (containsForward)
			forwardRenderObjects.push_back(renderer);

		return *renderer;
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

	std::list<MeshRenderer*>::iterator RenderScene::renderObjectsBegin() {
		return deferredRenderObjects.begin();
	}

	std::list<MeshRenderer*>::iterator RenderScene::renderObjectsEnd() {
		return deferredRenderObjects.end();
	}


	void RenderScene::iterRenderObjects(RenderMode mode, std::function<void(MeshRenderer* object)> function) {
		if(mode == RenderMode::All || mode == RenderMode::Deferred)
			for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);

		if(mode == RenderMode::All || mode == RenderMode::Forward)
			for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
		
		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
	}

	void RenderScene::iterRenderObjects(RenderMode mode, std::function<void(MeshRenderer* object)> function) const {
		if (mode == RenderMode::All || mode == RenderMode::Deferred)
			for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);

		if (mode == RenderMode::All || mode == RenderMode::Forward)
			for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);

		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
	}

}