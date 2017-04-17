#include "../texturing/brdfIntMap.h"
#include "irrmap.h"
#include "prefilterEnvmap.h"
#include "iblmap.h"

namespace geeL {

	IBLMap::IBLMap(BRDFIntegrationMap& brdfIntMap, IrradianceMap& irrMap, PrefilteredEnvironmentMap& envMap)
		: brdfIntMap(brdfIntMap), irrMap(irrMap), envMap(envMap) {}


	void IBLMap::bind(const Shader& shader, std::string name) const {
		irrMap.bind(shader, name);
	}

	void IBLMap::add(Shader& shader, std::string name) const {
		brdfIntMap.add(shader, name + "integration");
		irrMap.add(shader, name);
		envMap.add(shader, name);
	}

}