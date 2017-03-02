#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <utility>
#include <string>

namespace geeL {

	class Shader;

	enum MaterialType {
		Opaque = 0,
		Cutout = 1,
		Transparent = 2
	};


	class MaterialContainer {

	public:
		const MaterialType type;
		const std::string name;

		MaterialContainer(std::string name = "material", MaterialType type = Opaque)
			: type(type), name(name) {}

		virtual void bindTextures(Shader& shader) const = 0;
		virtual void bind(Shader& shader) const = 0;

	};


	class Material {
		
	public:
		Shader& shader;
		MaterialContainer& container;

		Material(Shader& shader, MaterialContainer& container, bool deferred = true);

		void bindTextures() const;
		void bind() const;

		bool rendersDeferred() const;

	private:
		bool deferred;

	};
}

#endif
