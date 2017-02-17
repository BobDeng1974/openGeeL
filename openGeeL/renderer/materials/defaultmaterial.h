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
	class DefaultMaterial : public Material {


	public:
		DefaultMaterial(Shader& defaultShader, MaterialType type = Opaque);

		void addTexture(std::string name, SimpleTexture& texture);
		void addTextures(std::vector<SimpleTexture*> textures);
		void setRoughness(float value);
		void setMetallic(float value);
		void setColor(glm::vec3 value);

		virtual void bindTextures() const;
		virtual void bind() const;

	private:
		glm::vec3 color;
		float roughness;
		float metallic;
		LayeredTexture textureStack;

	};
}

#endif