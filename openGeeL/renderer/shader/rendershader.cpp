#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "shaderreader.h"
#include "rendershader.h"

using namespace std;

namespace geeL {
	
	RenderShader::RenderShader()
		: Shader() {}

	RenderShader::RenderShader(const char* vertexPath, const char* fragmentPath,
		const char* geometryPath, ShaderProvider* const provider) 
			: Shader() {

		list<const StringReplacement*> list;

		name = fragmentPath;
		init(vertexPath, geometryPath, fragmentPath, provider, list);
	}


	void RenderShader::init(const char* vertexPath, const char* geometryPath, const char* fragmentPath, 
		ShaderProvider* const provider, std::list<const StringReplacement*>& replacements) {

		//Vertex shader
		string vertexCode = FileReader::readFile(vertexPath);
		ShaderFileReader::preprocessShaderString(*this, vertexCode, vertexPath, provider);
		const GLchar* vShaderCode = vertexCode.c_str();

		GLuint vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}


		//Fragment shader
		string fragmentCode = FileReader::readFile(fragmentPath);
		ShaderFileReader::preprocessShaderString(*this, fragmentCode, fragmentPath, provider);

		for (auto it(replacements.begin()); it != replacements.end(); it++)
			FileReader::replaceOccurence(fragmentCode, **it);

		const GLchar* fShaderCode = fragmentCode.c_str();

		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}

		//Geometry shader
		GLuint geometryShader;
		if (geometryPath != nullptr) {
			string geometryCode = FileReader::readFile(geometryPath);
			ShaderFileReader::preprocessShaderString(*this, geometryCode, geometryPath, provider);
			const GLchar* gShaderCode = geometryCode.c_str();

			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

			glShaderSource(geometryShader, 1, &gShaderCode, NULL);
			glCompileShader(geometryShader);

			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
				cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
			}
		}
		

		//Put progam together
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		if (geometryPath != nullptr) glAttachShader(program, geometryShader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		if (geometryPath != nullptr) glDeleteShader(geometryShader);
	}

}

