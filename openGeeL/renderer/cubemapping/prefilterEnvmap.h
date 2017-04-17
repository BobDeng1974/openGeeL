#ifndef PREFILTERENVMAP_H
#define PREFILTERENVMAP_H

#include "cubemap.h"

namespace geeL {

	class Shader;

	//Cubemap that stores pre-filtered environment map for splitsum approximated IBL
	class PrefilteredEnvironmentMap : public CubeMap {

	public:
		const CubeMap& environmentMap;

		PrefilteredEnvironmentMap(const CubeMap& environmentMap, unsigned int resolution = 512);
		~PrefilteredEnvironmentMap();

		virtual void add(Shader& shader, std::string name) const;

	private:
		Shader* conversionShader;
		unsigned int resolution;
		unsigned int fbo, rbo;

		void convertEnvironmentMap();

	};
}

#endif
