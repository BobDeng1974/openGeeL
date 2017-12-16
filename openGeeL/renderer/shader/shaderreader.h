#ifndef SHADERREADER_H
#define SHADERREADER_H

#include <string>

namespace geeL {

	class Shader;
	class ShaderProvider;


	struct StringReplacement {
		std::string expression;
		std::string replacement;
		size_t groupNumber;

		StringReplacement(const std::string& regex,
			const std::string& replacement,
			size_t groupNumber = 1)
				: expression(regex)
				, replacement(replacement)
				, groupNumber(groupNumber) {}

	};


	class FileReader {

	public:
		//Read code from file
		static std::string readFile(const char* path);

		static void replaceOccurence(std::string& shaderCode, const StringReplacement& replacement);

	};


	class ShaderFileReader {

	public:

		//Apply preprocessing to shader code. Specifically, include directives with the 
		//form '#include <path/to/file.glsl'  will be replaced with content of actual file
		//Note: Nesting is allowed and possible duplicates will be removed. However, multiple 
		//variable/funcition definitions won't be detected. Non-existing files will be simply ignored
		static std::string preprocessShaderString(Shader& shader, const std::string& shaderCode, const std::string& shaderPath, 
			ShaderProvider * const provider);

		//Apply preprocessing to shader code. Specifically, include directives with the 
		//form '#include <path/to/file.glsl'  will be replaced with content of actual file
		//Note: Nesting is allowed and possible duplicates will be removed. However, multiple 
		//variable/funcition definitions won't be detected. Non-existing files will be simply ignored
		static void preprocessShaderString(Shader& shader, std::string& shaderCode, const std::string& shaderPath,
			ShaderProvider * const provider);

		static void setGlobalVariables(std::string& shaderCode);

		
	};

}

#endif
