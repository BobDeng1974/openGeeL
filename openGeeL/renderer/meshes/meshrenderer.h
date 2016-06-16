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
	Model& model;
	vector<Material*> customMaterials;
	const CullingMode faceCulling;

	MeshRenderer(Transform& transform, Model& model, CullingMode faceCulling = cullFront);

	void draw();
};

}

#endif
