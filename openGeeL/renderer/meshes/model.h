#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <map>

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
	Model::Model(std::string path) : path(path) {}

	void loadModel(MaterialFactory& factory);
	void draw(bool shade = true) const;
	void drawInstanced(bool shade = true) const;
	void draw(std::vector<Material*> customMaterials) const;
	void draw(std::map<unsigned int, Material*> customMaterials) const;

	std::vector<Mesh>::iterator meshesBegin();
	std::vector<Mesh>::iterator meshesEnd();
	const Mesh& getMesh(unsigned int index);
	int meshCount() const;


private:
	std::string path;
	std::vector<Mesh> meshes;
	std::string directory;

	void processNode(MaterialFactory& factory, aiNode* node, const aiScene* scene);
	Mesh processMesh(MaterialFactory& factory, aiMesh* mesh, const aiScene* scene);
};

}

#endif
