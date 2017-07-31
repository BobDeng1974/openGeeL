#include "texturing/brdfIntMap.h"
#include "irrmap.h"
#include "shader/rendershader.h"
#include "prefilterEnvmap.h"
#include "iblmap.h"

namespace geeL {

	IBLMap::IBLMap(BRDFIntegrationMap& brdfIntMap, IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
		: DynamicCubeMap(&irrMap.getTexture()), brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {
	
		update();
	}

	IBLMap::IBLMap(const IBLMap& map) 
		: DynamicCubeMap(map), brdfIntMap(map.brdfIntMap), irrMap(map.irrMap), preEnvMap(map.preEnvMap) {}


	void IBLMap::draw(const RenderShader& shader, std::string name) const {
		irrMap.draw(shader, name);
	}

	void IBLMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(brdfIntMap, "BRDFIntegrationMap");

		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}

	void IBLMap::update() {
		irrMap.update();
		preEnvMap.update();
	}


	DynamicIBLMap::DynamicIBLMap(DynamicCubeMap& environmentMap, BRDFIntegrationMap& brdfIntMap, 
		IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
			: DynamicCubeMap(&environmentMap.getTexture()), baseMap(environmentMap), 
				brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {}

	DynamicIBLMap::DynamicIBLMap(const DynamicIBLMap& map) 
		: DynamicCubeMap(map), baseMap(map.baseMap), brdfIntMap(map.brdfIntMap), irrMap(map.irrMap), preEnvMap(map.preEnvMap) {}

	void DynamicIBLMap::draw(const RenderShader& shader, std::string name) const {
		baseMap.draw(shader, name);
	}

	void DynamicIBLMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(brdfIntMap, "BRDFIntegrationMap");

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