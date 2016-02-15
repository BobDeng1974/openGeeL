#ifndef SHADER_H
#define SHADER_H

#include <glew.h>;

namespace geeL {

class Shader {

public:

	GLuint program;

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

	void use();

};


}

#endif
