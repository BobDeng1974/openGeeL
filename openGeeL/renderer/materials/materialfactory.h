#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <list>
#include <map>
#include <string>
#include "../texturing/simpletexture.h"
#include "../texturing/envmap.h"

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

		//Creates and returns a new texture map from given file path or 
		//returns an existing texture if it already uses this file
		TextureMap& CreateTexture(std::string filePath, bool linear = false,
			TextureType type = Diffuse, ColorType colorType = ColorRGBA, GLint wrapMode = GL_REPEAT, FilterMode filterMode = None);

		//Create and returns new environment map or returns existing one if file is already in use
		EnvironmentMap& CreateEnvironmentMap(std::string filePath);

		//Creates and returns a new material with default shading
		DefaultMaterialContainer& CreateMaterial();

		//Creates and returns a new materials with given shading
		GenericMaterialContainer& CreateMaterial(Shader& shader);

		//Creates and returns a new shader program with given file paths
		//for vertex and fragment shaders
		Shader& CreateShader(std::string vertexPath, std::string fragmentPath);

		//Decide whether default shader should use forward or deferred shading
		void setDefaultShader(bool deferred);
		
		std::list<MaterialContainer*>::iterator materialsBegin();
		std::list<MaterialContainer*>::iterator materialsEnd();

		std::list<MaterialContainer*>::const_iterator materialsBegin() const;
		std::list<MaterialContainer*>::const_iterator materialsEnd() const;
		
		std::list<Shader*>::iterator shadersBegin();
		std::list<Shader*>::iterator shadersEnd();

		std::map<std::string, TextureMap>::const_iterator texturesBegin() const;
		std::map<std::string, TextureMap>::const_iterator texturesEnd() const;

		//Returns default shader for forward rendering
		Shader& getForwardShader() const;

		//Returns default shader for deferred rendering
		Shader& getDeferredShader() const;

	private:
		Shader* forwardShader;
		Shader* deferredShader;
		std::list<MaterialContainer*> container;
		std::list<Shader*> shaders;
		std::map<std::string, TextureMap> textures;
		std::map<std::string, EnvironmentMap> envMaps;

	};
}

#endif
