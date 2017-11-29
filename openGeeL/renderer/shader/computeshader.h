#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "shader.h"

namespace geeL {

	class ITexture;
	class ShaderProvider;


	class ComputeShader : public Shader {

	public:
		ComputeShader(const char* shaderPath, ShaderProvider* const provider = nullptr);

		//Add a new map as image texture to the shader
		void addImage(const ITexture& texture, unsigned int bindingPosition);

		//Load associated image texture into shader
		virtual void loadImages() const;

		static void invoke(unsigned int x, unsigned int y, unsigned int z);

	private:
		std::map<unsigned int, const ITexture*> images;

	};

}

#endif