#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include <utility>
#include <string>

namespace geeL {

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

	Material(Shader& shader, std::string name = "material", MaterialType type = Opaque) 
		: shader(shader), type(type), name(name) {}

	virtual void bindTextures() const = 0;
	virtual void bind() const = 0;

};

}

#endif
