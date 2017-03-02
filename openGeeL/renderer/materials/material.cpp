#include "material.h"

namespace geeL {

	Material::Material(Shader& shader, MaterialContainer& container, bool deferred)
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