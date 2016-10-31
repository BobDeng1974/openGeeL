#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <vector>
#include <map>
#include "../sceneobject.h"

namespace geeL {

class Material;
class DefaultMaterial;
class Model;
class Transform;

enum CullingMode {
	cullNone,
	cullFront,
	cullBack
};

class MeshRenderer : public SceneObject {

public:
	Model* model;
	const CullingMode faceCulling;

	//Constructor for mesh renderer with no assigned model (since it will be drawn instanced)
	MeshRenderer(Transform& transform, CullingMode faceCulling = cullFront);

	//Constructor for mesh renderer with an unique assigned model
	MeshRenderer(Transform& transform, Model& model, CullingMode faceCulling = cullFront);


	void draw(bool drawDefault = true) const;
	void draw(const Shader& shader) const;

	//Transform given model with transformation data of this mesh renderer
	void transformMeshes(Model& model, const Shader* shader = nullptr) const;

	//Change materials of mesh renderer in numerical order. For clarification:
	//Let materials hold i materials and default materials hold j materials
	//If i < j, then the first i materials will be changed
	//If i > j, then j materials will be change and the remaining (i - j) materials ignored
	void customizeMaterials(std::vector<Material*> materials);

	//Check if the meshes contain materials with non-default materials (meaning: with no default shading)
	bool containsNonDefaultMaterials() const;

	//Check if the meshes contain default materials (meaning: with default shading)
	bool containsDefaultMaterials() const;

private:
	bool instanced;
	std::map<unsigned int, Material*> defaultMaterials;
	std::map<unsigned int, Material*> customMaterials;

	void initMaterials();
	void transformMeshes(Model& model, const std::map<unsigned int, Material*>& materials, 
		const Shader* shader = nullptr) const;

};

}

#endif
