#include <fstream>
#include <sstream>
#include <iostream>
#include "shaderreader.h"

using namespace std;

namespace geeL {

	string ShaderFileReader::readShaderFile(const char * shaderPath) {
		string shaderCode;
		ifstream shaderFile;
		shaderFile.exceptions(ifstream::badbit);

		try {
			//Open files
			shaderFile.open(shaderPath);
			stringstream shaderStream;

			//Read file's buffer contents into streams
			shaderStream << shaderFile.rdbuf();

			//Close file handlers
			shaderFile.close();

			//Convert stream into string
			shaderCode = shaderStream.str();
		}
		catch (ifstream::failure e) {
			cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
		}

		return shaderCode;
	}

}