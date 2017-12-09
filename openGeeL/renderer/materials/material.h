#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <string>
#include <vec3.hpp>

namespace geeL {

	class SceneShader;
	class Texture2D;


	class MaterialContainer {

	public:
		std::string name;

		MaterialContainer(const std::string& name);
		virtual ~MaterialContainer() {}

		virtual void addTexture(const std::string& name, Texture2D& texture) = 0;

		virtual float getFloatValue(std::string name) const = 0;
		virtual int getIntValue(std::string name) const = 0;
		virtual glm::vec3 getVectorValue(std::string name) const = 0;

		virtual void setFloatValue(std::string name, float value) = 0;
		virtual void setIntValue(std::string name, int value) = 0;
		virtual void setVectorValue(std::string name, const glm::vec3& value) = 0;

		virtual void bind(SceneShader& shader) const = 0;

	};


	class Material {
		
	public:
		Material();
		Material(SceneShader& shader, MaterialContainer& container);

		//Copy other material but use given shader
		Material(const Material& other, SceneShader& shader);

		//Copy other material but use given material container
		Material(const Material& other, MaterialContainer& container);


		void bind() const;

		const SceneShader& getShader() const;
		SceneShader& getShader();
		MaterialContainer& getMaterialContainer() const;

	private:
		SceneShader* shader;
		MaterialContainer* container;

	};


	inline MaterialContainer::MaterialContainer(const std::string& name)
		: name(name) {}

}

#endif
