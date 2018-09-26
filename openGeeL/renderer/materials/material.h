#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <string>
#include <vec3.hpp>
#include "memory/memoryobject.h"

using namespace geeL::memory;

namespace geeL {

	class SceneShader;
	class ITexture;


	class MaterialContainer {

	public:
		std::string name;

		MaterialContainer(const std::string& name);
		virtual ~MaterialContainer() {}

		virtual void addTexture(const std::string& name, MemoryObject<ITexture> texture) = 0;

		virtual float getFloatValue(const std::string& name) const = 0;
		virtual int getIntValue(const std::string& name) const = 0;
		virtual glm::vec3 getVectorValue(const std::string& name) const = 0;

		virtual void setFloatValue(const std::string& name, float value) = 0;
		virtual void setIntValue(const std::string& name, int value) = 0;
		virtual void setVectorValue(const std::string& name, const glm::vec3& value) = 0;

		virtual void bind(SceneShader& shader) const = 0;

		virtual void iterTextures(std::function<void(const std::string&, const ITexture&)> function) const = 0;
		virtual size_t getTextureCount() const = 0;

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
		void loadMaps() const;

		const SceneShader& getShader() const;
		SceneShader& getShader();
		MaterialContainer& getMaterialContainer() const;

		void setShader(SceneShader& shader);

	private:
		SceneShader* shader;
		MaterialContainer* container;

	};


	inline MaterialContainer::MaterialContainer(const std::string& name)
		: name(name) {}

}

#endif
