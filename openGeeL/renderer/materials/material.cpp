#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "material.h"
#include "../shader/shader.h"
#include "../texturing/simpletexture.h"
#include "../cameras/camera.h"
#include "../lighting/lightmanager.h"
//#include "../transformation/transform.h"

namespace geeL {

	Material::Material(Shader& shader) : shader(shader), useCam(false), useLight(false) {
		textureStack = LayeredTexture();
	}

	void Material::addTexture(string name, SimpleTexture& texture) {
		textureStack.addTexture(name, texture);
	}

	void Material::addTextures(vector<SimpleTexture> textures) {

		for (size_t i = 0; i < textures.size(); i++) {
			SimpleTexture texture = textures[i];
			string name = texture.GetTypeAsString() + std::to_string(i);

			textureStack.addTexture(name, texture);
		}
	}

	void Material::useCamera(string name) {
		useCam = true;
		cam = name;
	}

	void Material::useLights(string point, string spot, string directional) {
		useLight = true;
		point = point;
		spot = spot;
		directional = directional;
	}

	void Material::addParameter(string name, float parameter) {
		staticFloats.push_back(pair<string, float>(name, parameter));
	}

	void Material::addParameter(string name, float* parameter) {
		dynamicFloats.push_back(pair<string, float*>(name, parameter));
	}

	void Material::addParameter(string name, int parameter) {
		staticInts.push_back(pair<string, int>(name, parameter));
	}

	void Material::addParameter(string name, int* parameter) {
		dynamicInts.push_back(pair<string, int*>(name, parameter));
	}

	void Material::addParameter(string name, vec3 parameter) {
		staticVec3s.push_back(pair<string, vec3>(name, parameter));
	}

	void Material::addParameter(string name, vec3* parameter) {
		dynamicVec3s.push_back(pair<string, vec3*>(name, parameter));
	}

	void Material::addParameter(string name, mat4 parameter) {
		staticMat4s.push_back(pair<string, mat4>(name, parameter));
	}

	void Material::addParameter(string name, mat4* parameter) {
		dynamicMat4s.push_back(pair<string, mat4*>(name, parameter));
	}

	void Material::staticBind(const LightManager& lightManager) const {

		shader.use();

		textureStack.bind(shader, "");
		
		//lightManager.bind(shader);


		GLint location;
		for (size_t i = 0; i < staticFloats.size(); i++) {
			pair<string, float> pair = staticFloats[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1f(location, pair.second);
		}

		for (size_t i = 0; i < staticInts.size(); i++) {
			pair<string, int> pair = staticInts[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1i(location, pair.second);
		}

		for (size_t i = 0; i < staticVec3s.size(); i++) {
			pair<string, vec3> pair = staticVec3s[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform3f(location, pair.second.x, pair.second.y, pair.second.z);
		}

		for (size_t i = 0; i < staticMat4s.size(); i++) {
			pair<string, mat4> pair = staticMat4s[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(pair.second));
		}
	}



	void Material::dynamicBind(const LightManager& lightManager, const Camera& currentCamera) const {

		shader.use();
		textureStack.draw();

		GLint location;
		for (size_t i = 0; i < dynamicFloats.size(); i++) {
			pair<string, float*> pair = dynamicFloats[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1f(location, *pair.second);
		}

		for (size_t i = 0; i < dynamicInts.size(); i++) {
			pair<string, int*> pair = dynamicInts[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform1i(location, *pair.second);
		}

		for (size_t i = 0; i < dynamicVec3s.size(); i++) {
			pair<string, vec3*> pair = dynamicVec3s[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniform3f(location, pair.second->x, pair.second->y, pair.second->z);
		}

		for (size_t i = 0; i < dynamicMat4s.size(); i++) {
			pair<string, mat4*> pair = dynamicMat4s[i];

			location = glGetUniformLocation(shader.program, pair.first.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(*pair.second));
		}

	}

}