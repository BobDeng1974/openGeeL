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
		~BRDFIntegrationMap();

		virtual unsigned int getID() const;
		virtual void remove();

	private:
		unsigned int id;

		BRDFIntegrationMap(const BRDFIntegrationMap& other) = delete;
		BRDFIntegrationMap& operator= (const BRDFIntegrationMap& other) = delete;

	};
}

#endif

