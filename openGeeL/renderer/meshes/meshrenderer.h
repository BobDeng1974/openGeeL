#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <vector>

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

class MeshRenderer {

public:
	Transform& transform;
	Model* model;
	vector<Material*> customMaterials;
	const CullingMode faceCulling;

	//Constructor for mesh renderer with no assigned model (since it will be drawn instanced)
	MeshRenderer(Transform& transform, CullingMode faceCulling = cullFront);

	//Constructor for mesh renderer with an unique assigned model
	MeshRenderer(Transform& transform, Model& model, CullingMode faceCulling = cullFront);

	//Draw models material and its gemoetry if it has an assigned model
	void draw();

	//Transform given model with transformation data of this mesh renderer
	void transformMeshes(Model& model);

private:
	bool instanced;

};

}

#endif
