#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "shaderreader.h"
#include "rendershader.h"

using namespace std;

namespace geeL {

	RenderShader::RenderShader(const char* vertexPath, const char* fragmentPath, 
		ShaderProvider* const provider) : Shader() {

		name = fragmentPath;
		init(vertexPath, fragmentPath, provider);
	}


	RenderShader::RenderShader(const char* vertexPath, const char* geometryPath, 
		const char* fragmentPath, ShaderProvider* const provider) : Shader() {

		name = fragmentPath;
		init(vertexPath, geometryPath, fragmentPath, provider);
	}


	void RenderShader::init(const char* vertexPath, const char* fragmentPath, ShaderProvider* const provider) {
		string vertexCode = FileReader::readFile(vertexPath);
		string fragmentCode = FileReader::readFile(fragmentPath);

		ShaderFileReader::preprocessShaderString(*this, vertexCode, vertexPath, provider);
		ShaderFileReader::preprocessShaderString(*this, fragmentCode, fragmentPath, provider);

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

		//Compile shaders
		//Vertex shader
		GLuint vertexShader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertexPath << "\n" << infoLog << endl;
		}

		//Fragment shader
		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragmentPath << "\n" << infoLog << endl;
		}

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void RenderShader::init(const char* vertexPath, const char* geometryPath, const char* fragmentPath, ShaderProvider* const provider) {
		string vertexCode = FileReader::readFile(vertexPath);
		string geometryCode = FileReader::readFile(geometryPath);
		string fragmentCode = FileReader::readFile(fragmentPath);

		ShaderFileReader::preprocessShaderString(*this, vertexCode, vertexPath, provider);
		ShaderFileReader::preprocessShaderString(*this, geometryCode, geometryPath, provider);
		ShaderFileReader::preprocessShaderString(*this, fragmentCode, fragmentPath, provider);

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* gShaderCode = geometryCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();

		//Compile shaders
		//Vertex shader
		GLuint vertexShader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(vertexShader, 1, &vShaderCode, NULL);
		glCompileShader(vertexShader);

		GLint success;
		GLchar infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
		}

		//Fragment shader
		GLuint fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
		}

		//Geometry shader
		GLuint geometryShader;
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		glShaderSource(geometryShader, 1, &gShaderCode, NULL);
		glCompileShader(geometryShader);

		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
			cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << endl;
		}

		//Put progam together
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glAttachShader(program, geometryShader);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(geometryShader);
	}

}

