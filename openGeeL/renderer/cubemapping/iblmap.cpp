#include "texturing/brdfIntMap.h"
#include "irrmap.h"
#include "shader/rendershader.h"
#include "prefilterEnvmap.h"
#include "iblmap.h"

namespace geeL {

	IBLMap::IBLMap(BRDFIntegrationMap& brdfIntMap, 
		IrradianceMap& irrMap, 
		PrefilteredEnvironmentMap& preEnvMap)
			: DynamicCubeMap(&irrMap.getTexture())
			, brdfIntMap(brdfIntMap)
			, irrMap(irrMap)
			, preEnvMap(preEnvMap) {
	
		draw();
	}

	IBLMap::IBLMap(const IBLMap& map) 
		: DynamicCubeMap(map), brdfIntMap(map.brdfIntMap), irrMap(map.irrMap), preEnvMap(map.preEnvMap) {}


	void IBLMap::bindMap(const RenderShader& shader, std::string name) const {
		irrMap.bindMap(shader, name);
	}

	void IBLMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(brdfIntMap, "BRDFIntegrationMap");

		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}

	void IBLMap::draw() {
		irrMap.draw();
		preEnvMap.draw();
	}

	const IrradianceMap& IBLMap::getIrradianceMap() const {
		return irrMap;
	}


	DynamicIBLMap::DynamicIBLMap(DynamicCubeMap& environmentMap, BRDFIntegrationMap& brdfIntMap, 
		IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap)
			: DynamicCubeMap(&environmentMap.getTexture()), baseMap(environmentMap), 
				brdfIntMap(brdfIntMap), irrMap(irrMap), preEnvMap(preEnvMap) {}

	DynamicIBLMap::DynamicIBLMap(const DynamicIBLMap& map) 
		: DynamicCubeMap(map), baseMap(map.baseMap), brdfIntMap(map.brdfIntMap), irrMap(map.irrMap), preEnvMap(map.preEnvMap) {}


	void DynamicIBLMap::bind(const Camera& camera, const RenderShader& shader, 
		const std::string& name, ShaderTransformSpace space) const {

		baseMap.bind(camera, shader, name, space);
	}

	void DynamicIBLMap::draw(const RenderShader& shader, std::string name) const {
		baseMap.bindMap(shader, name);
	}

	void DynamicIBLMap::add(RenderShader& shader, std::string name) const {
		shader.addMap(brdfIntMap, "BRDFIntegrationMap");

		baseMap.add(shader, name);
		irrMap.add(shader, name);
		preEnvMap.add(shader, name);
	}

	void DynamicIBLMap::draw() {
		baseMap.draw();
		irrMap.draw();
		preEnvMap.draw();
	}

	const IrradianceMap& DynamicIBLMap::getIrradianceMap() const {
		return irrMap;
	}

	const CubeMap& DynamicIBLMap::getEnvironmentMap() const {
		return baseMap;
	}

}