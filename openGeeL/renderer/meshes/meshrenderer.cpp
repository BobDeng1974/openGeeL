#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include "../materials/defaultmaterial.h"
#include "../materials/material.h"
#include "../transformation/transform.h"
#include "../shader/sceneshader.h"
#include "mesh.h"
#include "model.h"
#include "meshrenderer.h"

using namespace std;

namespace geeL{

	MeshRenderer::MeshRenderer(Transform& transform, SceneShader& shader, Model& model, CullingMode faceCulling, const std::string& name)
		: SceneObject(transform, name), model(&model), faceCulling(faceCulling) {
	
		initMaterials(shader);
	}


	void MeshRenderer::draw() const {
		cullFaces();

		for (auto it = materials.begin(); it != materials.end(); it++) {
			//Activate and forward information to shader
			const Shader& shader = *it->first;
			shader.use();
			shader.setMat4("model", transform.getMatrix());

			const std::list<MaterialMapping>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bindTextures();
				mat.bind();

				//Draw mesh
				const Mesh& mesh = *container.mesh;
				mesh.draw();
			}
		}

		uncullFaces();
	}

	void MeshRenderer::draw(const SceneShader& shader) const {
		cullFaces();

		auto it = materials.find(&shader);
		if (it != materials.end()) {
			const Shader& shader = *it->first;
			shader.use();
			shader.setMat4("model", transform.getMatrix());

			const std::list<MaterialMapping>& elements = it->second;
			for (auto et = elements.begin(); et != elements.end(); et++) {
				const MaterialMapping& container = *et;

				//Draw individual material
				const Material& mat = container.material;
				mat.bindTextures();
				mat.bind();

				//Draw mesh
				const Mesh& mesh = *container.mesh;
				mesh.draw();
			}
		}

		uncullFaces();
	}

	void MeshRenderer::drawExclusive(const Shader& shader) const {
		cullFaces();

		shader.use();
		shader.setMat4("model", transform.getMatrix());

		model->draw();

		uncullFaces();
	}
	

	void MeshRenderer::changeMaterial(Material&& material, const Mesh& mesh) {

		//Remove old element from materials since new material probably uses a different shader
		std::list<MaterialMapping>* elements;
		MaterialMapping* toRemove = nullptr;
		for (auto it = materials.begin(); it != materials.end(); it++) {
			elements = &it->second;
			for (auto et = elements->begin(); et != elements->end(); et++) {
				MaterialMapping& container = *et;

				if (container.mesh == &mesh) {
					toRemove = &container;
					break;
				}
			}

			if (toRemove != nullptr) {
				elements->remove(*toRemove);

				//Add moved 'copy' of new material
				const SceneShader& shader = material.getShader();
				materials[&shader].push_back(MaterialMapping(mesh, material));

				return;
			}
		}
	}

	void MeshRenderer::initMaterials(SceneShader& shader) {
		model->iterateMeshes([&](const Mesh& mesh) {
			MaterialContainer& container = mesh.getMaterialContainer();
			Material material = Material(shader, container);

			materials[&shader].push_back(MaterialMapping(mesh, material));
		});
	}


	void MeshRenderer::cullFaces() const {
		switch (faceCulling) {
			case CullingMode::cullNone:
				glDisable(GL_CULL_FACE);
			case CullingMode::cullBack:
				glCullFace(GL_BACK);
		}
	}

	void MeshRenderer::uncullFaces() const {
		switch (faceCulling) {
			case CullingMode::cullNone:
				glEnable(GL_CULL_FACE);
			case CullingMode::cullBack:
				glCullFace(GL_FRONT);
		}
	}

	void MeshRenderer::iterateMaterials(std::function<void(MaterialContainer&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const std::list<MaterialMapping>& elements = it->second;

			for (auto et = elements.begin(); et != elements.end(); et++) {
				const Material& mat = (*et).material;
				function(mat.getMaterialContainer());
			}
		}
	}

	void MeshRenderer::iterateShaders(std::function<void(const SceneShader&)> function) const {
		for (auto it = materials.begin(); it != materials.end(); it++) {
			const SceneShader& shader = *it->first;
			function(shader);
		}
	}


	const Model& MeshRenderer::getModel() const {
		return *model;
	}

	RenderMode MeshRenderer::getRenderMode() const {
		return RenderMode::Static;
	}
}