#include "../texturing/brdfIntMap.h"
#include "irrmap.h"
#include "prefilterEnvmap.h"
#include "iblmap.h"

namespace geeL {

	IBLMap::IBLMap(BRDFIntegrationMap& brdfIntMap, IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
		: brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {
	
		update();
	}

	IBLMap::IBLMap(const IBLMap& map) 
		: brdfIntMap(map.brdfIntMap), irrMap(map.irrMap), preEnvMap(map.preEnvMap) {}


	void IBLMap::bind(const Shader& shader, std::string name) const {
		irrMap.bind(shader, name);
	}

	void IBLMap::add(Shader& shader, std::string name) const {
		brdfIntMap.add(shader, "BRDFIntegrationMap");
		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}

	void IBLMap::update() {
		irrMap.update();
		preEnvMap.update();
	}


	DynamicIBLMap::DynamicIBLMap(DynamicCubeMap& environmentMap, BRDFIntegrationMap& brdfIntMap, 
		IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
			: baseMap(environmentMap), brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {}

	DynamicIBLMap::DynamicIBLMap(const DynamicIBLMap& map) 
		: baseMap(map.baseMap), brdfIntMap(map.brdfIntMap), irrMap(map.irrMap), preEnvMap(map.preEnvMap) {}

	void DynamicIBLMap::bind(const Shader& shader, std::string name) const {
		baseMap.bind(shader, name);
	}

	void DynamicIBLMap::add(Shader& shader, std::string name) const {
		brdfIntMap.add(shader, "BRDFIntegrationMap");
		baseMap.add(shader, name);
		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}

	void DynamicIBLMap::update() {
		baseMap.update();
		irrMap.update();
		preEnvMap.update();
	}

	const CubeMap& DynamicIBLMap::getEnvironmentMap() const {
		return baseMap;
	}

}