#include "renderer/renderer.h"
#include "envcubemap.h"
#include "irrmap.h"
#include "reflectionprobe.h"
#include "iblmap.h"
#include "prefilterEnvmap.h"
#include "cubemapfactory.h"

namespace geeL {

	CubeMapFactory::CubeMapFactory(CubeBuffer& buffer, 
		const Renderer& renderer,
		BRDFIntegrationMap& integrationMap)
			: buffer(buffer)
			, renderer(renderer)
			, integrationMap(integrationMap) {}


	CubeMapFactory::~CubeMapFactory() {
		for (auto it = cubeMaps.begin(); it != cubeMaps.end(); it++) {
			CubeMap* map = *it;
			delete map;
		}
	}

	CubeBuffer& CubeMapFactory::getBuffer() {
		return buffer;
	}


	EnvironmentCubeMap& CubeMapFactory::createEnvironmentCubeMap(EnvironmentMap& environmentMap, 
		unsigned int resolution) {

		EnvironmentCubeMap* map = new EnvironmentCubeMap(environmentMap, getBuffer(), resolution);
		cubeMaps.push_back(map);

		return *map;
	}

	EnvironmentCubeMap& CubeMapFactory::createEnvironmentCubeMap(const std::string& fileName, unsigned int resolution) {
		EnvironmentCubeMap* map = new EnvironmentCubeMap(fileName, getBuffer(), resolution);
		cubeMaps.push_back(map);

		return *map;
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

		std::function<void(const Camera&)> renderCall = [&](const Camera& camera) { renderer.drawForward(camera, false); };

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