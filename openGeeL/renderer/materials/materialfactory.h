#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <list>
#include <map>
#include <string>
#include "../texturing/simpletexture.h"

using namespace std;

namespace geeL {

	class Material;
	class MaterialContainer;
	class DefaultMaterialContainer;
	class GenericMaterialContainer;
	class Shader;

	class MaterialFactory {

	public:
		Shader* defaultShader;

		MaterialFactory();
		~MaterialFactory();

		//Creates and returns a new texture from given file path or 
		//returns an existing texture if it already uses this file
		SimpleTexture& CreateTexture(string filePath, bool linear = false, 
			TextureType type = Diffuse, ColorType colorType = ColorRGBA, GLint wrapMode = GL_REPEAT, FilterMode filterMode = None);

		//Creates and returns a new material with default shading
		DefaultMaterialContainer& CreateMaterial();

		//Creates and returns a new materials with given shading
		GenericMaterialContainer& CreateMaterial(Shader& shader);

		//Creates and returns a new shader program with given file paths
		//for vertex and fragment shaders
		Shader& CreateShader(string vertexPath, string fragmentPath);

		//Decide whether default shader should use forward or deferred shading
		void setDefaultShader(bool deferred);
		
		list<MaterialContainer*>::iterator materialsBegin();
		list<MaterialContainer*>::iterator materialsEnd();

		list<MaterialContainer*>::const_iterator materialsBegin() const;
		list<MaterialContainer*>::const_iterator materialsEnd() const;
		
		list<Shader*>::iterator shadersBegin();
		list<Shader*>::iterator shadersEnd();

		map<string, SimpleTexture>::const_iterator texturesBegin() const;
		map<string, SimpleTexture>::const_iterator texturesEnd() const;

		//Returns default shader for forward rendering
		Shader& getForwardShader() const;

		//Returns default shader for deferred rendering
		Shader& getDeferredShader() const;

	private:
		Shader* forwardShader;
		Shader* deferredShader;
		list<MaterialContainer*> container;
		list<Shader*> shaders;
		map<string, SimpleTexture> textures;

	};
}

#endif
