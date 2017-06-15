#include "../shader/sceneshader.h"
#include "../texturing/imagetexture.h"
#include "../texturing/envmap.h"
#include "genericmaterial.h"
#include "defaultmaterial.h"
#include "material.h"
#include "materialfactory.h"

using namespace std;

namespace geeL {

	MaterialFactory::MaterialFactory() : 
		forwardShader(new SceneShader("renderer/shaders/lighting.vert", FragmentShader("renderer/shaders/lighting.frag"), 
			ShaderTransformSpace::World, false)),
		deferredShader(new SceneShader("renderer/shaders/gbuffer.vert", FragmentShader("renderer/shaders/gbuffer.frag", false), 
			ShaderTransformSpace::View, false)),
		deferredAnimatedShader(new SceneShader("renderer/shaders/gbufferanim.vert", FragmentShader("renderer/shaders/gbuffer.frag", false), 
			ShaderTransformSpace::View, true)) {
	
		shaders.push_back(forwardShader);
	}

	MaterialFactory::~MaterialFactory() {
		delete deferredShader;
		delete deferredAnimatedShader;

		for (auto shader = shaders.begin(); shader != shaders.end(); shader++)
			delete *shader;

		for (auto material = container.begin(); material != container.end(); material++)
			delete *material;

		for (auto it = textures.begin(); it != textures.end(); it++) {
			ImageTexture& tex = it->second;
			tex.remove();
		}

		for (auto it = textureMaps.begin(); it != textureMaps.end(); it++) {
			TextureMap& tex = it->second;
			tex.remove();
		}

		for (auto it = envMaps.begin(); it != envMaps.end(); it++) {
			EnvironmentMap& tex = it->second;
			tex.remove();
		}
	}


	ImageTexture& MaterialFactory::CreateTexture(std::string filePath, ColorType colorType,
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter) {

		if (textures.find(filePath) == textures.end())
			textures[filePath] = ImageTexture(filePath.c_str(), colorType, wrapMode, filterMode, filter);

		return textures[filePath];
	}

	TextureMap& MaterialFactory::CreateTextureMap(string filePath, MapType type, ColorType colorType, 
		WrapMode wrapMode, FilterMode filterMode, AnisotropicFilter filter) {
		
		if (textureMaps.find(filePath) == textureMaps.end())
			textureMaps[filePath] = TextureMap(filePath.c_str(),  type, colorType, wrapMode, filterMode, filter);

		return textureMaps[filePath];
	}

	EnvironmentMap& MaterialFactory::CreateEnvironmentMap(string filePath) {
		if (envMaps.find(filePath) == envMaps.end())
			envMaps[filePath] = EnvironmentMap(filePath);

		return envMaps[filePath];
	}

	DefaultMaterialContainer& MaterialFactory::CreateMaterial() {
		DefaultMaterialContainer* mat = new DefaultMaterialContainer();
		container.push_back(mat);

		return *mat;
	}

	GenericMaterialContainer& MaterialFactory::CreateMaterial(RenderShader& shader) {
		GenericMaterialContainer* mat = new GenericMaterialContainer();
		container.push_back(mat);

		return *mat;
	}

	SceneShader& MaterialFactory::CreateShader(DefaultShading shading, string fragmentPath) {
		bool animated = false;
		std::string vertexPath;

		switch (shading) {
		case DefaultShading::DeferredStatic:
			vertexPath = "renderer/shaders/gbuffer.vert";
			break;
		case DefaultShading::DeferredSkinned:
			vertexPath = "renderer/shaders/gbufferanim.vert";
			animated = true;
			break;
		case DefaultShading::ForwardStatic:
			vertexPath = "renderer/shaders/lighting.vert";
			break;
		case DefaultShading::ForwardSkinned:
			vertexPath = "renderer/shaders/lighting.vert"; //TODO: create actual shader
			animated = true;
			break;
		}

		FragmentShader frag = FragmentShader(fragmentPath);

		ShaderTransformSpace space = (shading == DefaultShading::DeferredSkinned) || (shading == DefaultShading::DeferredStatic) ?
			ShaderTransformSpace::View : ShaderTransformSpace::World;

		shaders.push_back(new SceneShader(vertexPath, frag, space, animated));
		return *shaders.back();
	}
	
	list<MaterialContainer*>::iterator MaterialFactory::materialsBegin() {
		return container.begin();
	}

	list<MaterialContainer*>::iterator MaterialFactory::materialsEnd() {
		return container.end();
	}

	list<MaterialContainer*>::const_iterator MaterialFactory::materialsBegin() const {
		return container.begin();
	}

	list<MaterialContainer*>::const_iterator MaterialFactory::materialsEnd() const {
		return container.end();
	}
	
	list<SceneShader*>::iterator MaterialFactory::shadersBegin() {
		return shaders.begin();
	}

	list<SceneShader*>::iterator MaterialFactory::shadersEnd()  {
		return shaders.end();
	}

	map<string, TextureMap>::const_iterator MaterialFactory::texturesBegin() const {
		return textureMaps.begin();
	}

	map<string, TextureMap>::const_iterator MaterialFactory::texturesEnd() const {
		return textureMaps.end();
	}

	SceneShader& MaterialFactory::getDefaultShader(DefaultShading shading) const {
		switch (shading) {
			case DefaultShading::DeferredStatic:
				return *deferredShader;
			case DefaultShading::DeferredSkinned:
				return *deferredAnimatedShader;
			case DefaultShading::ForwardStatic:
				return *forwardShader;
			case DefaultShading::ForwardSkinned:
				//TODO: implement animated forward shader
				return *forwardShader;
		}

		return *deferredShader;
	}

	SceneShader& MaterialFactory::getForwardShader() const {
		return *forwardShader;
	}

	SceneShader& MaterialFactory::getDeferredShader() const {
		return *deferredShader;
	}

}