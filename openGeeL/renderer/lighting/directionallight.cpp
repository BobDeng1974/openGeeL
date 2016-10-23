#define GLEW_STATIC
#include <glew.h>
#include <string>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../shader/shader.h"
#include "../transformation/transform.h"
#include "directionallight.h"
#include "../scene.h"

using namespace std;
using namespace glm;

namespace geeL {

	DirectionalLight::DirectionalLight(Transform& transform, vec3 diffuse, vec3 specular, vec3 ambient, 
		float intensity, float shadowBias)
		: 
		Light(transform, diffuse, specular, ambient, intensity, shadowBias) {}


	void DirectionalLight::bind(const Shader& shader, int index, string name) const {
		Light::bind(shader, index, name);

		GLuint program = shader.program;
		string location = name + "[" + to_string(index) + "].";
		glUniform3f(glGetUniformLocation(program, (location + "direction").c_str()), 
			transform.forward.x, transform.forward.y, transform.forward.z);

		location = "direLightMatrix[" + to_string(index) + "]";
		glUniformMatrix4fv(glGetUniformLocation(shader.program, location.c_str()), 1, GL_FALSE,
			glm::value_ptr(lightTransform));
	}

	void DirectionalLight::computeLightTransform() {
		float a = shadowmapWidth / 100.f;
		mat4 projection = ortho(-a, a, -a, a, 1.0f, 50.f);
		mat4 view = lookAt(transform.forward, vec3(0.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f));
		
		lightTransform = projection * view;
	}
}