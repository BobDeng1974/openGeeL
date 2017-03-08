#ifndef GENERICMATERIAL_H
#define GENERICMATERIAL_H

#include <utility>
#include <map>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../texturing/layeredtexture.h"
#include "material.h"

using glm::vec3;
using glm::mat4;

namespace geeL {

	class SimpleTexture;
	class Shader;

	//A generic material class that allow a wide array of parameters
	class GenericMaterialContainer : public MaterialContainer {

	public:
		GenericMaterialContainer(std::string name = "material", MaterialType type = Opaque);

		void addTexture(std::string name, SimpleTexture& texture);
		void addTextures(std::vector<SimpleTexture*> textures);

		void addParameter(std::string name, float parameter);
		void addParameter(std::string name, int parameter);
		void addParameter(std::string name, vec3 parameter);
		void addParameter(std::string name, mat4 parameter);

		void bindTextures(Shader& shader) const;
		void bind(Shader& shader) const;

		float getFloatValue(std::string name) const;
		int   getIntValue(std::string name) const;
		vec3  getVectorValue(std::string name) const;
		mat4  getMatrixValue(std::string name) const;

		void getFloatValue(std::string name, float value) ;
		void setIntValue(std::string name, int value);
		void setVectorValue(std::string name, vec3 value);
		void setMatrixValue(std::string name, mat4 value);


	private:
		LayeredTexture textureStack;

		std::map<std::string, float> floatParameters;
		std::map<std::string, int> intParameters;
		std::map<std::string, vec3> vec3Parameters;
		std::map<std::string, mat4> mat4Parameters;

	};
}

#endif
