#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <utility>
#include <list>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../texturing/layeredtexture.h"


using glm::vec3;
using glm::mat4;

namespace geeL {

class LightManager;
class Camera;
class SimpleTexture;
class Shader;

enum MaterialType {
	Opaque = 0,
	Cutout = 1,
	Transparent = 2
};

class Material {

public:
	Shader& shader;
	const MaterialType type;
	const std::string name;

	Material(Shader& shader, string name = "material", MaterialType type = Opaque);

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
