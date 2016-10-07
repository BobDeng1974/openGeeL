#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>

using namespace std;

enum   aiTextureType;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiScene;

#include "../materials/material.h"

namespace geeL {

enum TextureType;

class Mesh;
class Texture;
class SimpleTexture;
class Material;
class Camera;
class LightManager;
class MaterialFactory;
class Transform;

class Model {
	
public:
	Model::Model() {}
	Model::Model(string path) : path(path) {}

	void loadModel(MaterialFactory& factory);
	void draw(bool shade = true) const;
	void drawInstanced(bool shade = true) const;
	void draw(vector<Material*> customMaterials) const;

	vector<Mesh>::iterator meshesBegin();
	vector<Mesh>::iterator meshesEnd();

private:
	string path;
	vector<Mesh> meshes;
	string directory;

	void processNode(MaterialFactory& factory, aiNode* node, const aiScene* scene);
	Mesh processMesh(MaterialFactory& factory, aiMesh* mesh, const aiScene* scene);
};

}

#endif
