#ifndef MATERIALFACTORY_H
#define MATERIALFACTORY_H

#include <list>
#include <map>
#include <string>
#include "../texturing/imagetexture.h"
#include "../texturing/envmap.h"
#include "../shader/defshading.h"

namespace geeL {

	class Material;
	class MaterialContainer;
	class DefaultMaterialContainer;
	class GenericMaterialContainer;
	class SceneShader;

	class MaterialFactory {

	public:
		MaterialFactory();
		~MaterialFactory();

		//Creates and returns a new texture from given file path or 
		//returns an existing texture if it already uses this file
		ImageTexture& CreateTexture(std::string filePath, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::Nearest, 
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Creates and returns a new texture map from given file path or 
		//returns an existing texture if it already uses this file
		TextureMap& CreateTextureMap(std::string filePath,
			MapType type = MapType::Diffuse, ColorType colorType = ColorType::RGBA,
			WrapMode wrapMode = WrapMode::Repeat, FilterMode filterMode = FilterMode::Nearest,
			AnisotropicFilter filter = AnisotropicFilter::Medium);

		//Create and returns new environment map or returns existing one if file is already in use
		EnvironmentMap& CreateEnvironmentMap(std::string filePath);

		//Creates and returns a new material with default shading
		DefaultMaterialContainer& CreateMaterial();

		//Creates and returns a new materials with given shading
		GenericMaterialContainer& CreateMaterial(RenderShader& shader);

		//Creates and returns a new shader program with given file paths
		//for vertex and fragment shaders
		SceneShader& CreateShader(DefaultShading shading, std::string fragmentPath);

		std::list<MaterialContainer*>::iterator materialsBegin();
		std::list<MaterialContainer*>::iterator materialsEnd();

		std::list<MaterialContainer*>::const_iterator materialsBegin() const;
		std::list<MaterialContainer*>::const_iterator materialsEnd() const;
		
		std::list<SceneShader*>::iterator shadersBegin();
		std::list<SceneShader*>::iterator shadersEnd();

		std::map<std::string, TextureMap>::const_iterator texturesBegin() const;
		std::map<std::string, TextureMap>::const_iterator texturesEnd() const;

		SceneShader& getDefaultShader(DefaultShading shading) const;

		//Returns default shader for static forward rendering
		SceneShader& getForwardShader() const;

		//Returns default shader for static deferred rendering
		SceneShader& getDeferredShader() const;

	private:
		SceneShader* forwardShader;
		SceneShader* deferredShader;
		SceneShader* deferredAnimatedShader;
		std::list<MaterialContainer*> container;
		std::list<SceneShader*> shaders;
		std::map<std::string, ImageTexture> textures;
		std::map<std::string, TextureMap> textureMaps;
		std::map<std::string, EnvironmentMap> envMaps;

	};
}

#endif
