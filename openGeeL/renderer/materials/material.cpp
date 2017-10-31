#include "material.h"

namespace geeL {

	Material::Material() {}

	Material::Material(SceneShader& shader, MaterialContainer& container)
		: shader(&shader), container(&container) {}

	Material::Material(const Material & other, SceneShader& shader)
		: shader(&shader), container(other.container) {}

	Material::Material(const Material & other, MaterialContainer& container)
		: shader(other.shader), container(&container) {}


	void Material::bind() const {
		container->bind(*shader);
	}

	const SceneShader& Material::getShader() const {
		return *shader;
	}

	SceneShader& Material::getShader() {
		return *shader;
	}

	MaterialContainer& Material::getMaterialContainer() const {
		return *container;
	}

}