#ifndef IBLMAP_H
#define IBLMAP_H

#include "cubemap.h"

namespace geeL {

	class BRDFIntegrationMap;
	class IrradianceMap;
	class PrefilteredEnvironmentMap;

	//Container class that contains irradiance map, pre-filtered environment map
	//and BRDF integration map for image based lighting
	class IBLMap : public DynamicCubeMap {

	public:
		IBLMap(BRDFIntegrationMap& brdfIntMap, IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap);
		IBLMap(const IBLMap& map);

		virtual void bindMap(const RenderShader& shader, std::string name) const;
		virtual void add(RenderShader& shader, std::string name) const;
		virtual void draw();

	private:
		BRDFIntegrationMap& brdfIntMap;
		IrradianceMap& irrMap;
		PrefilteredEnvironmentMap& preEnvMap;

	};

	//IBL map for use with a dynamic environment map
	class DynamicIBLMap : public DynamicCubeMap {

	public:
		DynamicIBLMap(DynamicCubeMap& environmentMap, BRDFIntegrationMap& brdfIntMap, 
			IrradianceMap& irrMap, PrefilteredEnvironmentMap& preEnvMap);
		DynamicIBLMap(const DynamicIBLMap& map);

		virtual void draw(const RenderShader& shader, std::string name) const;
		virtual void add(RenderShader& shader, std::string name) const;
		virtual void draw();

		const CubeMap& getEnvironmentMap() const;

	private:
		DynamicCubeMap& baseMap;
		BRDFIntegrationMap& brdfIntMap;
		IrradianceMap& irrMap;
		PrefilteredEnvironmentMap& preEnvMap;

	};

}

#endif
