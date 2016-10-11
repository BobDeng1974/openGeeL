#include "../shader/shader.h"
#include "material.h"
#include "materialfactory.h"


namespace geeL {

	MaterialFactory::MaterialFactory() 
		: defaultShader(new Shader("renderer/shaders/lighting.vert", "renderer/shaders/lighting.frag")) {
	
		shaders.push_back(defaultShader);
	}

	MaterialFactory::~MaterialFactory() {
		for (list<Shader*>::iterator it = shaders.begin(); it != shaders.end(); it++) {
			Shader* shader = *it;
			delete shader;
		}
	}

	SimpleTexture& MaterialFactory::CreateTexture(string filePath, 
		bool linear, TextureType type, GLint wrapMode, GLint filterMode) {
		
		if (textures.find(filePath) == textures.end())
			textures[filePath] = SimpleTexture(filePath.c_str(), linear, type, wrapMode, filterMode);

		return textures[filePath];
	}

	Material& MaterialFactory::CreateMaterial() {
		return CreateMaterial(*defaultShader);
	}

	Material& MaterialFactory::CreateMaterial(Shader& shader) {
		materials.push_back(Material(shader));
		//materials.back().staticBind();

		return materials.back();
	}

	Shader& MaterialFactory::CreateShader(string vertexPath, string fragmentPath) {
		shaders.push_back(new Shader(vertexPath.c_str(), fragmentPath.c_str()));
		return *shaders.back();
	}

	list<Material>::iterator MaterialFactory::materialsBegin() {
		return materials.begin();
	}

	list<Material>::iterator MaterialFactory::materialsEnd() {
		return materials.end();
	}

	list<Material>::const_iterator MaterialFactory::materialsBegin() const {
		return materials.begin();
	}

	list<Material>::const_iterator MaterialFactory::materialsEnd() const {
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

}