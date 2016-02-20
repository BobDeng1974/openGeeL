#ifndef SHADER_H
#define SHADER_H

namespace geeL {

class Shader {

public:
	int program;

	Shader() {}
	Shader(const char* vertexPath, const char* fragmentPath);

	void use() const;
};


}

#endif

