#ifndef RENDERSHADER_H
#define RENDERSHADER_H

#include "shader.h"

namespace geeL {

	class ShaderProvider;

	//Shader for the use in OpenGL rendering pipeline
	class RenderShader : public Shader {

	public:
		RenderShader() : Shader() {}

		RenderShader(const char* vertexPath, const char* fragmentPath, 
			ShaderProvider* const provider = nullptr);

		RenderShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath, 
			ShaderProvider* const provider = nullptr);

	protected:
		void init(const char* vertexPath, const char* fragmentPath, 
			ShaderProvider* const provider);

		void init(const char* vertexPath, const char* geometryPath, const char* fragmentPath, 
			ShaderProvider* const provider);

	};
}

#endif

