#include "irrmap.h"
#include "reflectionprobe.h"
#include "iblmap.h"
#include "prefilterEnvmap.h"
#include "cubemapfactory.h"

namespace geeL {

	CubeMapFactory::CubeMapFactory(CubeBuffer& buffer, ReflectionProbeRender renderCall, BRDFIntegrationMap& integrationMap)
		: buffer(buffer), renderCall(renderCall), integrationMap(integrationMap) {}


	CubeMapFactory::~CubeMapFactory() {
		for (auto it = cubeMaps.begin(); it != cubeMaps.end(); it++) {
			CubeMap* map = *it;
			delete map;
		}
	}

	CubeBuffer & CubeMapFactory::getBuffer() {
		return buffer;
	}


	IrradianceMap& CubeMapFactory::createIrradianceMap(const CubeMap& environmentMap,
		unsigned int resolution) {

		IrradianceMap* map = new IrradianceMap(environmentMap, buffer, resolution);
		cubeMaps.push_back(map);

		return *map;
	}

	PrefilteredEnvironmentMap& CubeMapFactory::createPrefilteredEnvironmentMap(const CubeMap& environmentMap,
		unsigned int resolution) {

		PrefilteredEnvironmentMap* map = new PrefilteredEnvironmentMap(environmentMap, buffer, resolution);
		cubeMaps.push_back(map);

		return *map;
	}

	IBLMap& CubeMapFactory::createIBLMap(const CubeMap& environmentMap) {
		IrradianceMap& irrMap = createIrradianceMap(environmentMap);
		PrefilteredEnvironmentMap& preMap = createPrefilteredEnvironmentMap(environmentMap);

		IBLMap* map = new IBLMap(integrationMap, irrMap, preMap);
		cubeMaps.push_back(map);

		return *map;
	}

	DynamicIBLMap& CubeMapFactory::createDynamicIBLMap(DynamicCubeMap& environmentMap) {
		IrradianceMap& irrMap = createIrradianceMap(environmentMap);
		PrefilteredEnvironmentMap& preMap = createPrefilteredEnvironmentMap(environmentMap);

		DynamicIBLMap* map = new DynamicIBLMap(environmentMap, integrationMap, irrMap, preMap);
		cubeMaps.push_back(map);

		return *map;
	}

	//Create reflection probe without 'Image Based Lighting'
	ReflectionProbe& CubeMapFactory::createReflectionProbe(Transform& transform, unsigned int resolution, float width,
		float height, float depth, std::string name) {

		ReflectionProbe* probe = new ReflectionProbe(buffer, renderCall, transform, resolution, width, height, depth, name);
		cubeMaps.push_back(probe);

		return *probe;
	}

	//Create reflection probe with 'Image Based Lighting'
	DynamicIBLMap& CubeMapFactory::createReflectionProbeIBL(Transform& transform, unsigned int resolution, float width,
		float height, float depth, std::string name) {

		ReflectionProbe& probe = createReflectionProbe(transform, resolution, width, height, depth, name);
		return createDynamicIBLMap(probe);
	}

}