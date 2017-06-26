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
		std::string name;

		MaterialContainer(std::string name = "material")
			: name(name) {}

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
		Material();
		Material(SceneShader& shader, MaterialContainer& container);

		void bindTextures() const;
		void bind() const;

		const SceneShader& getShader() const;
		SceneShader& getShader();
		MaterialContainer& getMaterialContainer() const;

	private:
		SceneShader* shader;
		MaterialContainer* container;

	};
}

#endif
