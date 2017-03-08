#ifndef DEFAULTMATERIAL_H
#define DEFAULTMATERIAL_H

#include <list>
#include <vector>
#include <string>
#include "../texturing/layeredtexture.h"
#include "material.h"

namespace geeL {

	class SimpleTexture;
	class Shader;

	//Material class with default shading
	class DefaultMaterialContainer : public MaterialContainer {


	public:
		glm::vec3 color;

		DefaultMaterialContainer(MaterialType type = Opaque);

		void addTexture(std::string name, SimpleTexture& texture);
		void addTextures(std::vector<SimpleTexture*> textures);

		float getRoughness() const;
		float getMetallic() const;
		glm::vec3 getColor() const;

		void setRoughness(float value);
		void setMetallic(float value);
		void setColor(glm::vec3 value);

		virtual void bindTextures(Shader& shader) const;
		virtual void bind(Shader& shader) const;

	private:
		
		float roughness;
		float metallic;
		LayeredTexture textureStack;

	};
}

#endif