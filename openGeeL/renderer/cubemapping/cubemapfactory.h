#ifndef CUBEMAPFACTORY_H
#define CUBEMAPFACTORY_H

#include <functional>
#include<list>

namespace geeL {

	class Camera;
	class CubeBuffer;
	class CubeMap;

	class BRDFIntegrationMap;
	class DynamicCubeMap;
	class DynamicIBLMap;
	class IBLMap;
	class IrradianceMap;
	class PrefilteredEnvironmentMap;
	class ReflectionProbe;
	class Transform;

	struct FrameBufferInformation;

	typedef std::function<void(const Camera&, FrameBufferInformation)> ReflectionProbeRender;

	class CubeMapFactory {

	public:
		CubeMapFactory(CubeBuffer& buffer, ReflectionProbeRender renderCall, BRDFIntegrationMap& integrationMap);
		~CubeMapFactory();

		IrradianceMap& createIrradianceMap(const CubeMap& environmentMap, unsigned int resolution = 32);

		PrefilteredEnvironmentMap& createPrefilteredEnvironmentMap(const CubeMap& environmentMap, 
			unsigned int resolution = 512);

		IBLMap& createIBLMap(const CubeMap& environmentMap);
		DynamicIBLMap& createDynamicIBLMap(DynamicCubeMap& environmentMap);

		//Create reflection probe without 'Image Based Lighting'
		ReflectionProbe& createReflectionProbe(Transform& transform, unsigned int resolution, float width = 50.f, 
			float height = 50.f, float depth = 50.f, std::string name = "ReflectionProbe");

		//Create reflection probe with 'Image Based Lighting'
		DynamicIBLMap& createReflectionProbeIBL(Transform& transform, unsigned int resolution, float width = 50.f,
			float height = 50.f, float depth = 50.f, std::string name = "ReflectionProbe");

	private:
		CubeBuffer& buffer;
		ReflectionProbeRender renderCall;
		BRDFIntegrationMap& integrationMap;
		std::list<CubeMap*> cubeMaps;

	};


}

#endif
