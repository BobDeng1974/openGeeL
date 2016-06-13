#include "../shader/shader.h"
#include "material.h"
#include "materialfactory.h"


namespace geeL {

	MaterialFactory::MaterialFactory() 
		: defaultShader(Shader("renderer/shaders/lighting.vert", "renderer/shaders/simpleDiffuse.frag")) {
	
		shaders.push_back(defaultShader);
	}


	SimpleTexture& MaterialFactory::CreateTexture(string filePath, TextureType type, GLint wrapMode, GLint filterMode) {
		if (textures.find(filePath) == textures.end())
			textures[filePath] = SimpleTexture(filePath.c_str(), type, wrapMode, filterMode);

		return textures[filePath];
	}

	Material& MaterialFactory::CreateMaterial() {
		return CreateMaterial(defaultShader);
	}

	Material& MaterialFactory::CreateMaterial(Shader& shader) {
		materials.push_back(Material(shader));
		return materials.back();
	}

	Shader& MaterialFactory::CreateShader(string vertexPath, string fragmentPath) {
		shaders.push_back(Shader(vertexPath.c_str(), fragmentPath.c_str()));
		return shaders.back();
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

	list<Shader>::const_iterator MaterialFactory::shadersBegin() const {
		return shaders.begin();
	}

	list<Shader>::const_iterator MaterialFactory::shadersEnd() const {
		return shaders.end();
	}

	map<string, SimpleTexture>::const_iterator MaterialFactory::texturesBegin() const {
		return textures.begin();
	}

	map<string, SimpleTexture>::const_iterator MaterialFactory::texturesEnd() const {
		return textures.end();
	}

}