#ifndef LIGHT_H
#define LIGHT_H

#include <vec3.hpp>
#include <string>

using namespace std;
using glm::vec3;

namespace geeL {

class Shader;

class Light {

public:
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	float intensity;

	Light(vec3 diffuse, vec3 specular, vec3 ambient, float intensity) 
		: diffuse(diffuse), specular(specular), ambient(ambient), intensity(intensity) {}

	virtual void bind(const Shader& shader, int index, string name) const;

};

}

#endif
