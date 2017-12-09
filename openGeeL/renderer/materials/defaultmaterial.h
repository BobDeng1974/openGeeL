#ifndef DEFAULTMATERIAL_H
#define DEFAULTMATERIAL_H

#include <list>
#include <vector>
#include <string>
#include "texturing/texturestack.h"
#include "material.h"

namespace geeL {

	class TextureMap;
	class SceneShader;

	//Material class with default shading
	class DefaultMaterialContainer : public MaterialContainer {

	public:
		DefaultMaterialContainer();

		virtual void addTexture(const std::string& name, MemoryObject<Texture2D> texture);
		void addTexture(MemoryObject<TextureMap> texture);
		void addTextures(std::vector<MemoryObject<TextureMap>> textures);
		void addTexture(const MapType& type, MemoryObject<Texture2D> texture);

		float getTransparency() const;
		float getRoughness() const;
		float getMetallic() const;
		const glm::vec3& getColor() const;
		const glm::vec3& getEmissivity() const;

		void setTransparency(float value);
		void setRoughness(float value);
		void setMetallic(float value);
		void setColor(const glm::vec3& value);
		void setEmissivity(const glm::vec3& value);

		virtual void bind(SceneShader& shader) const;

		virtual float getFloatValue(const std::string& name) const;
		virtual int getIntValue(const std::string& name) const;
		virtual glm::vec3 getVectorValue(const std::string& name) const;

		virtual void setFloatValue(const std::string& name, float value);
		virtual void setIntValue(const std::string& name, int value);
		virtual void setVectorValue(const std::string& name, const glm::vec3& value);

	private:
		glm::vec3 color, emissivity;
		float transparency;
		float roughness;
		float metallic;
		bool inverseRoughness = false;
		TextureStack textureStack;

	};
}

#endif