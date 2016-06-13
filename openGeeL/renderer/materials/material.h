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

class Material {

public:
	Shader& shader;

	Material(Shader& shader);

	void addTexture(string name, SimpleTexture& texture);
	void addTextures(vector<SimpleTexture> textures);

	void useCamera(string name);
	void useLights(string point, string spot, string directional);

	void addParameter(string name, float parameter);
	void addParameter(string name, float* parameter);

	void addParameter(string name, int parameter);
	void addParameter(string name, int* parameter);

	void addParameter(string name, vec3 parameter);
	void addParameter(string name, vec3* parameter);

	void addParameter(string name, mat4 parameter);
	void addParameter(string name, mat4* parameter);

	void staticBind(const LightManager& lightManager) const;
	void dynamicBind(const LightManager& lightManager, const Camera& currentCamera) const;

private:
	LayeredTexture textureStack;

	bool useCam, useLight;
	string cam, point, spot, directional;

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
