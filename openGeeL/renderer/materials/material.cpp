#include "material.h"

namespace geeL {

	Material::Material(SceneShader& shader, MaterialContainer& container, bool deferred)
		: shader(shader), container(container), deferred(deferred) {}


	void Material::bindTextures() const {
		container.bindTextures(shader);
	}

	void Material::bind() const {
		container.bind(shader);
	}

	bool Material::rendersDeferred() const {
		return deferred;
	}

}