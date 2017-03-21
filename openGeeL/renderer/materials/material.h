#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <string>
#include <vec3.hpp>

namespace geeL {

	class SceneShader;

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

		virtual void bindTextures(SceneShader& shader) const = 0;
		virtual void bind(SceneShader& shader) const = 0;

		virtual float getFloatValue(std::string name) const = 0;
		virtual int getIntValue(std::string name) const = 0;
		virtual glm::vec3 getVectorValue(std::string name) const = 0;

		virtual void setFloatValue(std::string name, float value) = 0;
		virtual void setIntValue(std::string name, int value) = 0;
		virtual void setVectorValue(std::string name, const glm::vec3& value) = 0;

	};


	class Material {
		
	public:
		SceneShader& shader;
		MaterialContainer& container;

		Material(SceneShader& shader, MaterialContainer& container, bool deferred = true);

		void bindTextures() const;
		void bind() const;

		bool rendersDeferred() const;

	private:
		bool deferred;

	};
}

#endif
