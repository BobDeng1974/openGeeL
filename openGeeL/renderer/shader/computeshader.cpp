#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "texturing/texture.h"
#include "shaderreader.h"
#include "computeshader.h"

using namespace std;

namespace geeL {

	ComputeShader::ComputeShader(const char* shaderPath, ShaderProvider* const provider) {
		name = shaderPath;

		string computeCode = ShaderFileReader::readShaderFile(shaderPath);
		ShaderFileReader::preprocessShaderString(*this, computeCode, shaderPath, provider);

		const GLchar* shaderCode = computeCode.c_str();

		unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);

		GLint success;
		GLchar infoLog[512];

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << endl;
		}

		program = glCreateProgram();
		glAttachShader(program, shader);
		glLinkProgram(program);
		
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		}

		glDeleteShader(shader);
	}


	void ComputeShader::addImage(const Texture& texture, unsigned int bindingPosition) {
		images[bindingPosition] = &texture;
	}

	void ComputeShader::loadImages() const {
		for (auto it = images.begin(); it != images.end(); it++) {
			unsigned int position = it->first;
			const Texture& texture = *it->second;
			
			texture.bindImage(position, AccessType::Read);
		}
	}

	void ComputeShader::invoke(unsigned int x, unsigned int y, unsigned int z) {
		glDispatchCompute(x, y, z);
	}

}