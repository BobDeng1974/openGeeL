#include "material.h"

namespace geeL {

	Material::Material() {}

	Material::Material(SceneShader& shader, MaterialContainer& container)
		: shader(&shader), container(&container) {}


	void Material::bindTextures() const {
		container->bindTextures(*shader);
	}

	void Material::bind() const {
		container->bind(*shader);
	}

	const SceneShader& Material::getShader() const {
		return *shader;
	}

	MaterialContainer& Material::getMaterialContainer() {
		return *container;
	}

}