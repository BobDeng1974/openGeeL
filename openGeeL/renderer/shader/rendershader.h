#ifndef RENDERSHADER_H
#define RENDERSHADER_H

#include "shader.h"

namespace geeL {

	//Shader for the use in OpenGL rendering pipeline
	class RenderShader : public Shader {

	public:
		RenderShader() : Shader() {}
		RenderShader(const char* vertexPath, const char* fragmentPath);
		RenderShader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

	protected:
		void init(const char* vertexPath, const char* fragmentPath);
		void init(const char* vertexPath, const char* geometryPath, const char* fragmentPath);

	};
}

#endif

