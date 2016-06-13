#ifndef SHADER_H
#define SHADER_H

#include <iostream>

namespace geeL {

class Shader {

public:
	int program;
	const bool useLight;
	const bool useCamera;

	Shader() : useLight(false), useCamera(false) {}
	Shader(const char* vertexPath, const char* fragmentPath, bool useLight = true, bool useCamera = true);

	void use() const;

};


}

#endif

