#ifndef IRRADIANCEMAP_H
#define IRRADIANCEMAP_H

#include "cubemap.h"

namespace geeL {

	class Shader;

	//Cubemap that stores irradiance from another cubemap
	class IrradianceMap : public CubeMap {

	public:
		const CubeMap& environmentMap;

		IrradianceMap(const CubeMap& environmentMap, unsigned int resolution = 32);
		~IrradianceMap();

		virtual void bind(const Shader& shader, std::string name) const;

	private:
		Shader* conversionShader;
		unsigned int resolution;
		unsigned int fbo;

		void convertEnvironmentMap();

	};
}

#endif
