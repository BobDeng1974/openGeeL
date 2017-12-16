#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <iostream>
#include "shaderprovider.h"
#include "shaderreader.h"

using namespace std;

typedef pair<size_t, string> IndexedMatch;


namespace geeL {

	string FileReader::readFile(const char * shaderPath) {
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

	void FileReader::replaceOccurence(std::string& code, const StringReplacement& replacement) {
		regex require(replacement.expression);

		smatch result;
		regex_search(code, result, require);

		string currentValue = result.str(replacement.groupNumber);
		for (sregex_iterator it(code.begin(), code.end(), require); it != sregex_iterator(); it++) {
			string current = (*it).str();

			string oldCurrent = current;

			size_t position = current.find(currentValue);
			if (position != std::string::npos) {
				current.replace(position, currentValue.length(), replacement.replacement);
			}

			code.replace(code.find(oldCurrent), oldCurrent.length(), current);
		}
	}




	void preprocessIncludes(std::string& file, set<string>& includedFiles) {
		regex include("^\\s*#\\s*include\\s+(?:<[^>]*>|\"[^\"]*\")\\s*");

		list<IndexedMatch> includeList;
		for (sregex_iterator it(file.begin(), file.end(), include); it != sregex_iterator(); it++)
			includeList.push_back(IndexedMatch((*it).position(), (*it).str()));

		//Order regex matches to guarantee correct include order 
		includeList.sort([](const IndexedMatch& a, const IndexedMatch& b) { return a.first < b.first; });


		for(auto it(includeList.begin()); it != includeList.end(); it++) {
			string current = it->second;
			regex repl(current);

			size_t startIndex = current.find("<");
			size_t offset = current.find(">") - startIndex;
			string filePath = current.substr(startIndex + 1, offset - 1);

			//File has already been included so the include is simply removed
			if (includedFiles.count(filePath))
				file = regex_replace(file, repl, "\n");
			//Recursivly add included file (if it exists)
			else {
				includedFiles.insert(filePath);

				string includeCode = FileReader::readFile(filePath.c_str());
				preprocessIncludes(includeCode, includedFiles);
				file.replace(file.find(current), current.length(), "\n" + includeCode + "\n\n");
			}
		}
		
	}

	void preprocessRequirements(Shader& shader, std::string& file, set<string>& requirements, ShaderProvider * const provider) {
		regex require("^#require\\s+[A-Z, \_]+\\s?");

		for (sregex_iterator it(file.begin(), file.end(), require); it != sregex_iterator(); it++) {
			string current = (*it).str();
			regex repl(current);

			string element = current.substr(9, current.length() - 9);
			element.erase(remove(element.begin(), element.end(), ' '), element.end());

			//File already contains this requirement and it is therefore simply removed
			//OR requirement can't be met since no provider is attached
			if (requirements.count(element) || provider == nullptr)
				file = regex_replace(file, repl, "");
			else {
				requirements.insert(element);

				ResourceInfo& info = provider->linkResource(element, shader);
				//Check if requested resource actually exists and then add uniform binding point
				//Linking will be automatically done by shader class
				if (info.exists) {
					string replacement = "uniform " + info.type + " " + element + ";";
					file.replace(file.find(current), current.length(), replacement + "\n\n");
				}
				else
					file = regex_replace(file, repl, "");
			}
		}
	}

	string ShaderFileReader::preprocessShaderString(Shader& shader, const string& shaderCode,
		const std::string& shaderPath, ShaderProvider * const provider) {
		
		std::string result = shaderCode;
		preprocessShaderString(shader, result, shaderPath, provider);

		return result;
	}

	void ShaderFileReader::preprocessShaderString(Shader& shader, std::string& shaderCode,
		const std::string& shaderPath, ShaderProvider * const provider) {
		
		set<string> requirements;
		preprocessRequirements(shader, shaderCode, requirements, provider);
		
		set<string> includedFiles;
		includedFiles.insert(shaderPath);
		preprocessIncludes(shaderCode, includedFiles);

		//Lazy fix: Run pre process again if there is at least one #include still found in code
		unsigned int counter = 0;
		while (shaderCode.find("#include") != string::npos && counter < 5) {
			preprocessIncludes(shaderCode, includedFiles);
			counter++;
		}

	}

	
}