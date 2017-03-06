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
#include "lighting\lightmanager.h"
#include "scene.h"

using namespace std;

namespace geeL {

	RenderScene::RenderScene(LightManager& lightManager, Camera& camera, MeshFactory& meshFactory) 
		: lightManager(lightManager), camera(camera), meshFactory(meshFactory) {}

	
	RenderScene::~RenderScene() {
		for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), [&](MeshRenderer* object) {
			delete object;
		});

		for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), [&](MeshRenderer* object) {
			delete object;
		});
	}


	void RenderScene::update() {
		camera.update();

		originViewSpace = TranslateToViewSpace(vec3(0.f, 0.f, 0.f));
	}


	void RenderScene::drawDeferred() const {
		drawObjects(deferredRenderObjects, true);
	}

	void RenderScene::drawForward() const {
		drawObjects(forwardRenderObjects, false);
	}

	void RenderScene::drawObjects(const std::list<MeshRenderer*>& objects, bool deferred) const {
		for_each(objects.begin(), objects.end(), [&](MeshRenderer* object) {
			object->draw(deferred);
		});
	}

	void RenderScene::draw() const {
		drawObjects(deferredRenderObjects, true);
		drawObjects(forwardRenderObjects, false);
		drawSkybox();
	}

	void RenderScene::drawObjects(const Shader& shader) const {
		shader.use();

		for_each(deferredRenderObjects.begin(), deferredRenderObjects.end(), [&](MeshRenderer* object) {
			object->draw(shader);
		});

		for_each(forwardRenderObjects.begin(), forwardRenderObjects.end(), [&](MeshRenderer* object) {
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
		CullingMode faceCulling, bool deferred) {
		
		Model& model = meshFactory.CreateModel(modelPath);
		if(deferred)
			deferredRenderObjects.push_back(meshFactory.CreateMeshRendererManual(model, transform, faceCulling, deferred));
		else
			forwardRenderObjects.push_back(meshFactory.CreateMeshRendererManual(model, transform, faceCulling, deferred));

		return *deferredRenderObjects.back();
	}

	MeshRenderer& RenderScene::AddMeshRenderer(std::string modelPath, Transform& transform,
		std::vector<Material*> materials, CullingMode faceCulling) {

		Model& model = meshFactory.CreateModel(modelPath);
		MeshRenderer* renderer = meshFactory.CreateMeshRendererManual(model, transform, faceCulling);
		renderer->customizeMaterials(materials);

		if (renderer->containsDeferredMaterials())
			deferredRenderObjects.push_back(renderer);
		if (renderer->containsForwardMaterials())
			forwardRenderObjects.push_back(renderer);

		return *renderer;
	}

	void RenderScene::forwardScreenInfo(const ScreenInfo& info) {
		camera.updateDepth(info);
		lightManager.forwardScreenInfo(info, camera);
	}

	glm::vec3 RenderScene::TranslateToScreenSpace(glm::vec3 vector) const {
		glm::vec4 vec = camera.getProjectionMatrix() * camera.getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x / vec.w, vec.y / vec.w, vec.z / vec.w) * 0.5f + 0.5f;
	}

	glm::vec3 RenderScene::TranslateToViewSpace(glm::vec3 vector) const {
		glm::vec4 vec = camera.getViewMatrix() * glm::vec4(vector, 1.f);
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::vec3 RenderScene::TranslateToWorldSpace(glm::vec3 vector) const {
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

	std::list<MeshRenderer*>::iterator RenderScene::renderObjectsBegin() {
		return deferredRenderObjects.begin();
	}

	std::list<MeshRenderer*>::iterator RenderScene::renderObjectsEnd() {
		return deferredRenderObjects.end();
	}

}