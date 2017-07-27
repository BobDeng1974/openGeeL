#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "shader.h"

namespace geeL {

	class ShaderProvider;


	class ComputeShader : public Shader {

	public:
		ComputeShader(const char* shaderPath, ShaderProvider* const provider = nullptr);

		virtual void loadMaps() const;
		virtual void loadMaps(unsigned int offset) const;

		static void invoke(unsigned int x, unsigned int y, unsigned int z);

	};

}

#endif