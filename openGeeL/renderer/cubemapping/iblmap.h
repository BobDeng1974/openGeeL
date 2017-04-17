#ifndef IBLMAP_H
#define IBLMAP_H

#include "cubemap.h"

namespace geeL {

	class BRDFIntegrationMap;
	class IrradianceMap;
	class PrefilteredEnvironmentMap;

	//Container class that contains irradiance map, pre-filtered environment map
	//and BRDF integration map for image based lighting
	class IBLMap : public CubeMap {

	public:
		IBLMap(BRDFIntegrationMap& brdfIntMap, IrradianceMap& irrMap, PrefilteredEnvironmentMap& envMap);

		virtual void bind(const Shader& shader, std::string name) const;
		virtual void add(Shader& shader, std::string name) const;

	private:
		BRDFIntegrationMap& brdfIntMap;
		IrradianceMap& irrMap;
		PrefilteredEnvironmentMap& envMap;

	};

}

#endif
