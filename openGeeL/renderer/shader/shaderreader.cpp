#include <fstream>
#include <regex>
#include <set>
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



	void preprocessIncludes(std::string& file, set<string>& includedFiles) {
		regex include("^\\s*#\\s*include\\s+(?:<[^>]*>|\"[^\"]*\")\\s*");

		for (sregex_iterator it(file.begin(), file.end(), include); it != sregex_iterator(); it++) {
			string current = (*it).str();
			regex repl(current);

			size_t startIndex = current.find("<");
			size_t offset = current.find(">") - startIndex;
			string filePath = current.substr(startIndex + 1, offset - 1);

			//File has already been included so the include is simply removed
			if (includedFiles.count(filePath))
				file = regex_replace(file, repl, "");
			//Recursivly add included file (if it exists)
			else {
				includedFiles.insert(filePath);

				string includeCode = ShaderFileReader::readShaderFile(filePath.c_str());
				preprocessIncludes(includeCode, includedFiles);
				file.replace(file.find(current), current.length(), includeCode + "\n\n");
			}
		}
	}

	string ShaderFileReader::preprocessShaderString(const string& shaderCode, const std::string& shaderPath) {
		std::string result = shaderCode;
		preprocessShaderString(result, shaderPath);

		return result;
	}

	void ShaderFileReader::preprocessShaderString(std::string& shaderCode, const std::string& shaderPath) {
		set<string> includedFiles;
		includedFiles.insert(shaderPath);

		preprocessIncludes(shaderCode, includedFiles);
	}

}