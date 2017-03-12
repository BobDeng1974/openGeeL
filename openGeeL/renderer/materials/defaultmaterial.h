#ifndef DEFAULTMATERIAL_H
#define DEFAULTMATERIAL_H

#include <list>
#include <vector>
#include <string>
#include "../texturing/layeredtexture.h"
#include "material.h"

namespace geeL {

	class TextureMap;
	class Shader;

	//Material class with default shading
	class DefaultMaterialContainer : public MaterialContainer {


	public:
		glm::vec3 color;

		DefaultMaterialContainer(MaterialType type = Opaque);

		void addTexture(std::string name, TextureMap& texture);
		void addTextures(std::vector<TextureMap*> textures);

		float getRoughness() const;
		float getMetallic() const;
		glm::vec3 getColor() const;

		void setRoughness(float value);
		void setMetallic(float value);
		void setColor(glm::vec3 value);

		virtual void bindTextures(Shader& shader) const;
		virtual void bind(Shader& shader) const;

		virtual float getFloatValue(std::string name) const;
		virtual int getIntValue(std::string name) const;
		virtual glm::vec3 getVectorValue(std::string name) const;

		virtual void setFloatValue(std::string name, float value);
		virtual void setIntValue(std::string name, int value);
		virtual void setVectorValue(std::string name, const glm::vec3& value);

	private:
		
		float roughness;
		float metallic;
		LayeredTexture textureStack;

	};
}

#endif