#ifndef IRRADIANCEMAP_H
#define IRRADIANCEMAP_H

#include "cubemap.h"

namespace geeL {

	class CubeBuffer;
	class Shader;

	//Cubemap that stores irradiance from another cubemap
	class IrradianceMap : public CubeMap {

	public:
		const CubeMap& environmentMap;

		IrradianceMap(const CubeMap& environmentMap, CubeBuffer& frameBuffer, unsigned int resolution = 32);
		~IrradianceMap();

		virtual void bind(const Shader& shader, std::string name) const;
		virtual void add(Shader& shader, std::string name) const;

	private:
		CubeBuffer& frameBuffer;
		Shader* conversionShader;
		unsigned int resolution;

		void convertEnvironmentMap();

	};
}

#endif
