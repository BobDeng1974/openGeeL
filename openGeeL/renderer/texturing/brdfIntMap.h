#ifndef BRDFINTEGRATIONMAP_H
#define BRDFINTEGRATIONMAP_H

#include <string>
#include "texture.h"

namespace geeL {

	class RenderShader;

	//BRDF integration map that stores information for splitsum approximated IBL
	class BRDFIntegrationMap : public Texture2D {

	public:
		BRDFIntegrationMap();

	private:
		BRDFIntegrationMap(const BRDFIntegrationMap& other) = delete;
		BRDFIntegrationMap& operator= (const BRDFIntegrationMap& other) = delete;

	};
}

#endif

