#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <utility>
#include <vector>
#include <string>
#include <vec3.hpp>
#include <mat4x4.hpp>
#include "../texturing/layeredtexture.h"

using namespace std;
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
	Transparent = 2	//TODO: doesn't really make sense right now because of unordered draw calls
};

class Material {

public:
	Shader& shader;
	const MaterialType type;
	const string name;

	Material(Shader& shader, string name = "material", MaterialType type = Opaque);

	void addTexture(string name, SimpleTexture& texture);
	void addTextures(vector<SimpleTexture*> textures);

	void addParameter(string name, float parameter);
	void addParameter(string name, float* parameter);

	void addParameter(string name, int parameter);
	void addParameter(string name, int* parameter);

	void addParameter(string name, vec3 parameter);
	void addParameter(string name, vec3* parameter);

	void addParameter(string name, mat4 parameter);
	void addParameter(string name, mat4* parameter);

	void staticBind() const;
	void dynamicBind() const;

private:
	LayeredTexture textureStack;

	vector<pair<string, float>> staticFloats;
	vector<pair<string, float*>> dynamicFloats;

	vector<pair<string, int>> staticInts;
	vector<pair<string, int*>> dynamicInts;

	vector<pair<string, vec3>> staticVec3s;
	vector<pair<string, vec3*>> dynamicVec3s;

	vector<pair<string, mat4>> staticMat4s;
	vector<pair<string, mat4*>> dynamicMat4s;
};


}



#endif
