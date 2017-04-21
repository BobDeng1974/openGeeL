#include "../texturing/brdfIntMap.h"
#include "irrmap.h"
#include "prefilterEnvmap.h"
#include "iblmap.h"

namespace geeL {

	IBLMap::IBLMap(BRDFIntegrationMap& brdfIntMap, IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
		: brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {}


	void IBLMap::bind(const Shader& shader, std::string name) const {
		irrMap.bind(shader, name);
	}

	void IBLMap::add(Shader& shader, std::string name) const {
		brdfIntMap.add(shader, name + "integration");
		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}


	DynamicIBLMap::DynamicIBLMap(DynamicCubeMap& environmentMap, BRDFIntegrationMap& brdfIntMap, 
		IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
			: baseMap(environmentMap), brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {}


	void DynamicIBLMap::bind(const Shader& shader, std::string name) const {
		baseMap.bind(shader, name);
	}

	void DynamicIBLMap::add(Shader& shader, std::string name) const {
		brdfIntMap.add(shader, name + "integration");
		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}

	void DynamicIBLMap::update() {
		baseMap.update();
		irrMap.update();
		preEnvMap.update();
	}

}