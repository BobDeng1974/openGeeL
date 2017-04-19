#define GLEW_STATIC
#include <glew.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "../texturing/texture.h"
#include "shader.h"

using namespace std;

namespace geeL {

	Shader::Shader(const char* vertexPath, const char* fragmentPath)
		: name(fragmentPath), mapBindingPos(0) {

		init(vertexPath, fragmentPath);
	}


	Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath)
		: name(fragmentPath), mapBindingPos(0) {

		init(vertexPath, geometryPath, fragmentPath);
	}


	static unsigned int activeShader = 0;
	void Shader::use() const {
		if (program != activeShader) {
			glUseProgram(program);
			activeShader = program;
		}
	}

	void Shader::addMap(TextureID id, const std::string& name, unsigned int type) {
		unsigned int offset = maps.size();
		glUniform1i(glGetUniformLocation(program, name.c_str()), mapOffset + offset);

		maps[id] = TextureBinding(id, type, offset, name);
		mapBindingPos++;
	}

	void Shader::addMap(const Texture& texture, const std::string& name, unsigned int type) {
		addMap(texture.getID(), name, type);
	}

	void Shader::removeMap(TextureID id) {
		auto it = maps.find(id);
		if (it != maps.end()) {
			maps.erase(it);

			//Rebind all maps again since positions might have changed
			bindMaps();
		}
	}

	void Shader::bindMaps() {
		int counter = 0;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			TextureBinding& binding = (*it).second;
			binding.offset = counter;
			glUniform1i(glGetUniformLocation(program, binding.name.c_str()), mapOffset + binding.offset);
			counter++;
		}

		mapBindingPos = maps.size() + mapOffset;
	}
	
	void Shader::loadMaps() const {
		int layer = GL_TEXTURE0;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			const TextureBinding& binding = (*it).second;
			glActiveTexture(layer + mapOffset + binding.offset);
			glBindTexture(binding.type, binding.id);
		}
	}

	void Shader::loadMaps(std::list<unsigned int>& maps, unsigned int type) const {
		int layer = GL_TEXTURE0;
		int counter = mapOffset;
		for (auto it = maps.begin(); it != maps.end(); it++) {
			glActiveTexture(layer + counter);

			glBindTexture(type, *it);
			counter++;
		}
	}

	ShaderLocation Shader::getLocation(std::string name) const {
		return glGetUniformLocation(program, name.c_str());
	}

	ShaderLocation Shader::setInteger(string name, int value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1i(glGetUniformLocation(program, name.c_str()), value);

		return location;
	}

	ShaderLocation Shader::setFloat(string name, float value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform1f(glGetUniformLocation(program, name.c_str()), value);

		return location;
	}

	ShaderLocation Shader::setVector3(string name, const glm::vec3& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniform3f(glGetUniformLocation(program, name.c_str()),
			value.x, value.y, value.z);

		return location;
	}

	ShaderLocation Shader::setMat4(string name, const glm::mat4& value) const {
		ShaderLocation location = glGetUniformLocation(program, name.c_str());
		glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE,
			glm::value_ptr(value));

		return location;
	}

	void Shader::setInteger(ShaderLocation location, int value) const {
		glUniform1i(location, value);
	}

	void Shader::setFloat(ShaderLocation location, float value) const {
		glUniform1f(location, value);
	}

	void Shader::setVector3(ShaderLocation location, const glm::vec3& value) const {
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::setMat4(ShaderLocation location, const glm::mat4& value) const {
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}


	void Shader::init(const char* vertexPath, const char* fragmentPath) {
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

	void Shader::init(const char* vertexPath, const char* geometryPath, const char* fragmentPath) {
		//Read code from file path
		string vertexCode;
		string fragmentCode;
		string geometryCode;
		ifstream vShaderFile;
		ifstream fShaderFile;
		ifstream gShaderFile;

		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		gShaderFile.exceptions(ifstream::badbit);

		try {
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			gShaderFile.open(geometryPath);
			stringstream vShaderStream, fShaderStream, gShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			gShaderStream << gShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			gShaderFile.close();
			// Convert stream into GLchar array
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			geometryCode = gShaderStream.str();
		}
		catch (ifstream::failure e) {
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		}

		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		const GLchar* gShaderCode = geometryCode.c_str();

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

