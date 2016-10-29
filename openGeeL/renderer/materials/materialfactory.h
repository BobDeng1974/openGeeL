#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <list>
#include <map>
#include <string>
#include "../texturing/simpletexture.h"

using namespace std;

namespace geeL {

class Material;
class DefaultMaterial;
class GenericMaterial;
class Shader;

class MaterialFactory {

public:
	Shader* defaultShader;

	MaterialFactory();
	~MaterialFactory();

	//Creates and returns a new texture from given file path or 
	//returns an existing texture if it already uses this file
	SimpleTexture& CreateTexture(string filePath, bool linear = false, 
		TextureType type = Diffuse, GLint wrapMode = GL_REPEAT, GLint filterMode = GL_LINEAR);

	//Creates and returns a new material with default shading
	DefaultMaterial& CreateMaterial();

	//Creates and returns a new materials with given shading
	GenericMaterial& CreateMaterial(Shader& shader);

	//Creates and returns a new shader program with given file paths
	//for vertex and fragment shaders
	Shader& CreateShader(string vertexPath, string fragmentPath);

	list<Material*>::iterator materialsBegin();
	list<Material*>::iterator materialsEnd();

	list<Material*>::const_iterator materialsBegin() const;
	list<Material*>::const_iterator materialsEnd() const;

	list<Shader*>::iterator shadersBegin();
	list<Shader*>::iterator shadersEnd();

	map<string, SimpleTexture>::const_iterator texturesBegin() const;
	map<string, SimpleTexture>::const_iterator texturesEnd() const;

private:
	list<Material*> materials;
	list<Shader*> shaders;
	map<string, SimpleTexture> textures;

};

}



#endif
