#ifndef PREFILTERENVMAP_H
#define PREFILTERENVMAP_H

#include "cubemap.h"

namespace geeL {

	class CubeBuffer;
	class Shader;

	//Cubemap that stores pre-filtered environment map for splitsum approximated IBL
	class PrefilteredEnvironmentMap : public DynamicCubeMap {

	public:
		const CubeMap& environmentMap;

		PrefilteredEnvironmentMap(const CubeMap& environmentMap, CubeBuffer& frameBuffer, unsigned int resolution = 512);
		~PrefilteredEnvironmentMap();

		virtual void add(Shader& shader, std::string name) const;
		virtual void update();

	private:
		Shader* conversionShader;
		CubeBuffer& frameBuffer;
		unsigned int resolution;

	};
}

#endif
