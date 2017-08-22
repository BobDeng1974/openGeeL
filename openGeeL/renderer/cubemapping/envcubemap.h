#ifndef ENVIRONMENTCUBEMAP_H
#define ENVIRONMENTCUBEMAP_H

#include <string>
#include "cubemap.h"

namespace geeL {

	class CubeBuffer;
	class RenderShader;
	class EnvironmentMap;

	//Cubemap that uses an environment map as a base
	class EnvironmentCubeMap : public CubeMap {

	public:
		EnvironmentCubeMap(const EnvironmentMap& map, CubeBuffer& frameBuffer, unsigned int resolution = 512);
		virtual ~EnvironmentCubeMap();

	private:
		const EnvironmentMap& map;
		CubeBuffer& frameBuffer;
		RenderShader* conversionShader;

		void draw();

	};
}

#endif
