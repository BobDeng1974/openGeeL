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

class MeshRenderer {

public:
	Transform& transform;
	Model& model;
	vector<Material*> customMaterials;

	MeshRenderer(Transform& transform, Model& model);

	void draw(const LightManager& lightManager, const Camera& currentCamera);

};

}

#endif
