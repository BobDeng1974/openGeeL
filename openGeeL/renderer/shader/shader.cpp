#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "shader.h"

using namespace std;

namespace geeL {

	Shader::Shader(const char* vertexPath, const char* fragmentPath, bool useLight, bool useCamera, bool useSkybox
		, string cam, string skybox, string pointlight, string spotLights, string directionalLights)
		: useLight(useLight), useCamera(useCamera), useSkybox(useSkybox), cam("camera"), skybox(skybox),
		point("pointLights"), spot("spotLights"), directional("directionalLights"), mapBindingPos(0) {

		//Read code from file path
		string vertexCode;
		string fragmentCode;
		ifstream vShaderFile;
		ifstream fShaderFile;

		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);

		try {
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// Convert stream into GLchar array
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e) {
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		}

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

	void Shader::use() const {
		glUseProgram(program);
	}

	void Shader::addMap(int mapID, string name) {
		maps.push_back(pair<int, string>(mapID, name));
	}

	void Shader::bindMaps() {
		int counter = 0;
		for (list<pair<int, string>>::const_iterator it = maps.begin(); it != maps.end(); it++) {
			glUniform1i(glGetUniformLocation(program, it->second.c_str()), counter);
			counter++;
		}

		mapBindingPos = maps.size();
	}

	void Shader::loadMaps() const {
		int layer = GL_TEXTURE0;
		int counter = 0;
		for (list<pair<int, string>>::const_iterator it = maps.begin(); it != maps.end(); it++) {
			glActiveTexture(layer + counter);
			glBindTexture(GL_TEXTURE_2D, it->first);
			counter++;
		}
	}
}

