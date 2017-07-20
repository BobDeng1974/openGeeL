#ifndef SHADERREADER_H
#define SHADERREADER_H

#include <string>

namespace geeL {

	class ShaderFileReader {

	public:
		//Read shader code from file
		static std::string readShaderFile(const char * shaderPath);

		//Apply preprocessing to shader code
		static std::string preprocessShaderString(const std::string& shaderCode, const std::string& shaderPath);
		static void preprocessShaderString(std::string& shaderCode, const std::string& shaderPath);

	};

}

#endif
