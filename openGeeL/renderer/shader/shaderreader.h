#ifndef SHADERREADER_H
#define SHADERREADER_H

#include <string>

namespace geeL {

	class ShaderFileReader {

	public:
		//Read shader code from file
		static std::string readShaderFile(const char * shaderPath);

		//Apply preprocessing to shader code. Specifically, include directives with the 
		//form '#include <path/to/file.glsl'  will be replaced with content of actual file
		//Note: Nesting is allowed and possible duplicates will be removed. However, multiple 
		//variable/funcition definitions won't be detected. Non-existing files will be simply ignored
		static std::string preprocessShaderString(const std::string& shaderCode, const std::string& shaderPath);

		//Apply preprocessing to shader code. Specifically, include directives with the 
		//form '#include <path/to/file.glsl'  will be replaced with content of actual file
		//Note: Nesting is allowed and possible duplicates will be removed. However, multiple 
		//variable/funcition definitions won't be detected. Non-existing files will be simply ignored
		static void preprocessShaderString(std::string& shaderCode, const std::string& shaderPath);

	};

}

#endif
