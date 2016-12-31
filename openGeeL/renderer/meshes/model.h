#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <map>

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

	void draw(bool shade = true) const;
	void drawInstanced(bool shade = true) const;
	void draw(std::vector<Material*> customMaterials) const;
	void draw(std::map<unsigned int, Material*> customMaterials) const;

	void addMesh(Mesh mesh);
	const Mesh& getMesh(unsigned int index);
	int meshCount() const;

	std::vector<Mesh>::iterator meshesBegin();
	std::vector<Mesh>::iterator meshesEnd();

private:
	std::string path;
	std::vector<Mesh> meshes;
	std::string directory;

};

}

#endif
