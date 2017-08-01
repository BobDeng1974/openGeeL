#ifndef IRRADIANCEMAP_H
#define IRRADIANCEMAP_H

#include "cubemap.h"

namespace geeL {

	class CubeBuffer;
	class RenderShader;

	//Cubemap that stores irradiance from another cubemap
	class IrradianceMap : public DynamicCubeMap {

	public:
		const CubeMap& environmentMap;

		IrradianceMap(const CubeMap& environmentMap, CubeBuffer& frameBuffer, unsigned int resolution = 32);
		~IrradianceMap();

		virtual void bindMap(const RenderShader& shader, std::string name) const;
		virtual void add(RenderShader& shader, std::string name) const;

		virtual void draw();

	private:
		CubeBuffer& frameBuffer;
		RenderShader* conversionShader;

	};
}

#endif
