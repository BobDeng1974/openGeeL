#ifndef GENERICMATERIAL_H
#define GENERICMATERIAL_H

#include <utility>
#include <vector>
#include <list>
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
	class GenericMaterial : public Material {

	public:
		GenericMaterial(Shader& shader, std::string name = "material", MaterialType type = Opaque);

		void addTexture(std::string name, SimpleTexture& texture);
		void addTextures(std::vector<SimpleTexture*> textures);

		void addParameter(std::string name, float parameter);
		void addParameter(std::string name, int parameter);
		void addParameter(std::string name, vec3 parameter);
		void addParameter(std::string name, mat4 parameter);

		void addParameter(std::string name, const float* parameter);
		void addParameter(std::string name, const int* parameter);
		void addParameter(std::string name, const vec3* parameter);
		void addParameter(std::string name, const mat4* parameter);

		void bindTextures() const;
		void bind() const;

	private:
		LayeredTexture textureStack;

		std::list<std::pair<std::string, float>> floatParameters;
		std::list<std::pair<std::string, int>> intParameters;
		std::list<std::pair<std::string, vec3>> vec3Parameters;
		std::list<std::pair<std::string, mat4>> mat4Parameters;

		std::list<std::pair<std::string, const float*>> unmanagedFloatParameters;
		std::list<std::pair<std::string, const int*>> unmanagedIntParameters;
		std::list<std::pair<std::string, const vec3*>> unmanagedVec3Parameters;
		std::list<std::pair<std::string, const mat4*>> unmanagedMat4Parameters;
	};
}

#endif
