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
		iterRenderObjects([&](MeshRenderer* object) {
			delete object;
		});
	}


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

	SkinnedMeshRenderer& RenderScene::AddSkinnedMeshRenderer(string modelPath, Transform& transform,
		CullingMode faceCulling, bool deferred, string name) {

		SkinnedModel& model = meshFactory.CreateSkinnedModel(modelPath);
		SkinnedMeshRenderer* renderer = meshFactory.CreateSkinnedMeshRendererManual(model, transform, faceCulling, deferred, name);

		if (deferred)
			deferredRenderObjects.push_back(renderer);
		else
			forwardRenderObjects.push_back(renderer);

		return *renderer;
	}

	SkinnedMeshRenderer& RenderScene::AddSkinnedMeshRenderer(std::string modelPath, Transform& transform,
		std::vector<Material*> materials, CullingMode faceCulling, string name) {

		SkinnedModel& model = meshFactory.CreateSkinnedModel(modelPath);
		SkinnedMeshRenderer* renderer = meshFactory.CreateSkinnedMeshRendererManual(model, transform, faceCulling, true, name);
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

	void RenderScene::iterRenderObjects(RenderObjectsMode mode, std::function<void(MeshRenderer*)> function) {
		if(mode == RenderObjectsMode::Deferred)
			for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);

		if(mode == RenderObjectsMode::Forward)
			for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
		
		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
	}

	void RenderScene::iterRenderObjects(RenderObjectsMode mode, std::function<void(const MeshRenderer*)> function) const {
		if (mode == RenderObjectsMode::Deferred)
			for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);

		if (mode == RenderObjectsMode::Forward)
			for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);

		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
	}

	void RenderScene::iterRenderObjects(std::function<void(MeshRenderer*)> function) {
		for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
		for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
	}

	void RenderScene::iterRenderObjects(std::function<void(const MeshRenderer*)> function) const {
		for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), function);
		for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), function);
		for_each(mixedRenderObjects.begin(), mixedRenderObjects.end(), function);
	}

}