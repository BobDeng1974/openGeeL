#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "shader.h"

namespace geeL {

	class ComputeShader : public Shader {

	public:
		ComputeShader(const char* shaderPath);


		static void invoke(unsigned int x, unsigned int y, unsigned int z);

	};

}

#endif