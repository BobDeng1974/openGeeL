#include "../shader/shader.h"
#include "genericmaterial.h"
#include "defaultmaterial.h"
#include "material.h"
#include "materialfactory.h"


namespace geeL {

	MaterialFactory::MaterialFactory() : 
		forwardShader(new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag")),
		deferredShader(new Shader("renderer/shaders/gbuffer.vert", "renderer/shaders/gbuffer.frag", false)) {
	
		defaultShader = forwardShader;
		shaders.push_back(forwardShader);
	}

	MaterialFactory::~MaterialFactory() {
		for (auto shader = shaders.begin(); shader != shaders.end(); shader++)
			delete *shader;

		for (auto material = container.begin(); material != container.end(); material++)
			delete *material;
	}

	SimpleTexture& MaterialFactory::CreateTexture(string filePath, 
		bool linear, TextureType type, ColorType colorType, GLint wrapMode, FilterMode filterMode) {
		
		if (textures.find(filePath) == textures.end())
			textures[filePath] = SimpleTexture(filePath.c_str(), linear, type, colorType, wrapMode, filterMode);

		return textures[filePath];
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

	Shader& MaterialFactory::CreateShader(string vertexPath, string fragmentPath) {
		shaders.push_back(new Shader(vertexPath.c_str(), fragmentPath.c_str()));
		return *shaders.back();
	}

	void MaterialFactory::setDefaultShader(bool deferred) {
		defaultShader = deferred ? deferredShader : forwardShader;
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
	
	list<Shader*>::iterator MaterialFactory::shadersBegin() {
		return shaders.begin();
	}

	list<Shader*>::iterator MaterialFactory::shadersEnd()  {
		return shaders.end();
	}

	map<string, SimpleTexture>::const_iterator MaterialFactory::texturesBegin() const {
		return textures.begin();
	}

	map<string, SimpleTexture>::const_iterator MaterialFactory::texturesEnd() const {
		return textures.end();
	}

	Shader& MaterialFactory::getForwardShader() const {
		return *forwardShader;
	}

	Shader& MaterialFactory::getDeferredShader() const {
		return *deferredShader;
	}

}