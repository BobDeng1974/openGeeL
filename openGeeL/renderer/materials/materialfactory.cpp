#include "../shader/shader.h"
#include "genericmaterial.h"
#include "defaultmaterial.h"
#include "material.h"
#include "materialfactory.h"


namespace geeL {

	MaterialFactory::MaterialFactory() 
		: 
		forwardShader(new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag")),
		deferredShader(new Shader("renderer/shaders/gbuffer.vert", "renderer/shaders/gbuffer.frag", false)) {
	
		defaultShader = forwardShader;
		shaders.push_back(forwardShader);
	}

	MaterialFactory::~MaterialFactory() {
		//delete defaultShader;
		//delete forwardShader;
		//delete deferredShader;

		for (list<Shader*>::iterator it = shaders.begin(); it != shaders.end(); it++) {
			Shader* shader = *it;
			delete shader;
		}

		for (list<Material*>::iterator it = materials.begin(); it != materials.end(); it++) {
			Material* material = *it;
			delete material;
		}
	}

	SimpleTexture& MaterialFactory::CreateTexture(string filePath, 
		bool linear, TextureType type, ColorType colorType, GLint wrapMode, FilterMode filterMode) {
		
		if (textures.find(filePath) == textures.end())
			textures[filePath] = SimpleTexture(filePath.c_str(), linear, type, colorType, wrapMode, filterMode);

		return textures[filePath];
	}

	DefaultMaterial& MaterialFactory::CreateMaterial() {
		DefaultMaterial* mat = new DefaultMaterial(*defaultShader);
		materials.push_back(mat);

		return *mat;
	}

	GenericMaterial& MaterialFactory::CreateMaterial(Shader& shader) {
		GenericMaterial* mat = new GenericMaterial(shader);
		materials.push_back(mat);

		return *mat;
	}

	Shader& MaterialFactory::CreateShader(string vertexPath, string fragmentPath) {
		shaders.push_back(new Shader(vertexPath.c_str(), fragmentPath.c_str()));
		return *shaders.back();
	}

	void MaterialFactory::setDefaultShader(bool deferred) {
		defaultShader = deferred ? deferredShader : forwardShader;
		//shaders.front() = defaultShader;
	}

	list<Material*>::iterator MaterialFactory::materialsBegin() {
		return materials.begin();
	}

	list<Material*>::iterator MaterialFactory::materialsEnd() {
		return materials.end();
	}

	list<Material*>::const_iterator MaterialFactory::materialsBegin() const {
		return materials.begin();
	}

	list<Material*>::const_iterator MaterialFactory::materialsEnd() const {
		return materials.end();
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