#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <vector>
#include "../sceneobject.h"

using namespace std;

namespace geeL {

class Material;
class Model;
class Transform;
class LightManager;
class Camera;

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

	//Draw models material(if parameter 'shade' is true) and its gemoetry (if it has an assigned model)
	void draw() const;
	void draw(const Shader& shader) const;

	//Transform given model with transformation data of this mesh renderer
	void transformMeshes(Model& model, const Shader* shader = nullptr) const;

	//Change materials of mesh renderer in numerical order. For clarification:
	//Let materials hold i materials and default materials hold j materials
	//If i < j, then the first i materials will be changed
	//If i > j, then j materials will be change and the remaining (i - j) materials ignored
	void customizeMaterials(vector<Material*> materials);

	vector<Material*>::const_iterator materialsBegin() const;
	vector<Material*>::const_iterator materialsEnd() const;


private:
	bool instanced;
	vector<Material*> customMaterials;

	void initMaterials();

};

}

#endif
