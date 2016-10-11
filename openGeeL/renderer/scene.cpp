#include "shader\shader.h"
#include "meshes\mesh.h"
#include "meshes\model.h"
#include "meshes\meshrenderer.h"
#include "meshes\meshfactory.h"
#include "cameras\camera.h"
#include "cubemapping\skybox.h"


#include "scene.h"

namespace geeL {

	RenderScene::RenderScene(LightManager& lightManager, Camera& camera, MeshFactory& meshFactory) 
		: lightManager(lightManager), camera(camera), meshFactory(meshFactory) {}


	void RenderScene::draw() const {

		camera.update();
		drawSkybox();
		drawObjects();
	}

	void RenderScene::drawObjects() const {
		for (std::list<MeshRenderer>::const_iterator it = renderObjects.begin(); 
			it != renderObjects.end(); it++) {
			
			it->draw();
		}
	}

	void RenderScene::drawObjects(const Shader& shader) const {
		shader.use();

		for (std::list<MeshRenderer>::const_iterator it = renderObjects.begin(); 
			it != renderObjects.end(); it++) {

			it->draw(shader);
		}
	}

	void RenderScene::setSkybox(Skybox& skybox) {
		this->skybox = &skybox;
	}

	void RenderScene::drawSkybox() const {
		if (skybox != nullptr)
			skybox->draw(camera);
	}

	void RenderScene::bindSkybox(const Shader& shader) const {
		if (skybox != nullptr)
			skybox->bind(shader);
	}

	MeshRenderer& RenderScene::AddMeshRenderer(string modelPath, Transform& transform, CullingMode faceCulling) {
		Model& model = meshFactory.CreateModel(modelPath);
		renderObjects.push_back(MeshRenderer(transform, model, faceCulling));

		return renderObjects.back();
	}

}