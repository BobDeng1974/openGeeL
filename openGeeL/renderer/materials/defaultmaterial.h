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

		virtual void addTexture(const std::string& name, Texture2D& texture);
		void addTexture(TextureMap& texture);
		void addTextures(std::vector<TextureMap*> textures);

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

		virtual float getFloatValue(std::string name) const;
		virtual int getIntValue(std::string name) const;
		virtual glm::vec3 getVectorValue(std::string name) const;

		virtual void setFloatValue(std::string name, float value);
		virtual void setIntValue(std::string name, int value);
		virtual void setVectorValue(std::string name, const glm::vec3& value);

	private:
		glm::vec3 color, emissivity;
		float transparency;
		float roughness;
		float metallic;
		TextureStack textureStack;

	};
}

#endif