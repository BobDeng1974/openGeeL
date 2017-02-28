#ifndef ENVIRONMENTCUBEMAP_H
#define ENVIRONMENTCUBEMAP_H

#include <string>
#include "cubemap.h"

namespace geeL {

	class Shader;
	class EnvironmentMap;

	//Cubemap that uses an environment map as a base
	class EnvironmentCubeMap : public CubeMap {

	public:
		EnvironmentCubeMap(const EnvironmentMap& map, unsigned int resolution = 512);
		~EnvironmentCubeMap();

		void bind(const Shader& shader, std::string name) const;

	private:
		const EnvironmentMap& map;
		Shader* conversionShader;
		unsigned int fbo;
		unsigned int resolution;

		void convertEnvironmentMap();

	};
}

#endif
