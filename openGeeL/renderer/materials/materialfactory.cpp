#include "../shader/shader.h"
#include "genericmaterial.h"
#include "defaultmaterial.h"
#include "material.h"
#include "materialfactory.h"

using namespace std;

namespace geeL {

	MaterialFactory::MaterialFactory() : 
		forwardShader(new SceneShader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag")),
		deferredShader(new SceneShader("renderer/shaders/gbuffer.vert", "renderer/shaders/gbuffer.frag", true, false)),
		deferredAnimatedShader(new SceneShader("renderer/shaders/gbufferanim.vert", "renderer/shaders/gbuffer.frag", true, false)) {
	
		shaders.push_back(forwardShader);
	}

	MaterialFactory::~MaterialFactory() {
		delete deferredShader;
		delete deferredAnimatedShader;

		for (auto shader = shaders.begin(); shader != shaders.end(); shader++)
			delete *shader;

		for (auto material = container.begin(); material != container.end(); material++)
			delete *material;
	}


	SimpleTexture& MaterialFactory::CreateTexture(std::string filePath, ColorType colorType,
		WrapMode wrapMode, FilterMode filterMode) {

		if (textures.find(filePath) == textures.end())
			textures[filePath] = SimpleTexture(filePath.c_str(), colorType, wrapMode, filterMode);

		return textures[filePath];
	}

	TextureMap& MaterialFactory::CreateTextureMap(string filePath, MapType type, ColorType colorType, 
		WrapMode wrapMode, FilterMode filterMode) {
		
		if (textureMaps.find(filePath) == textureMaps.end())
			textureMaps[filePath] = TextureMap(filePath.c_str(),  type, colorType, wrapMode, filterMode);

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

	GenericMaterialContainer& MaterialFactory::CreateMaterial(Shader& shader) {
		GenericMaterialContainer* mat = new GenericMaterialContainer();
		container.push_back(mat);

		return *mat;
	}

	SceneShader& MaterialFactory::CreateShader(string vertexPath, string fragmentPath) {
		shaders.push_back(new SceneShader(vertexPath.c_str(), fragmentPath.c_str()));
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
	}

	SceneShader& MaterialFactory::getForwardShader() const {
		return *forwardShader;
	}

	SceneShader& MaterialFactory::getDeferredShader() const {
		return *deferredShader;
	}

}