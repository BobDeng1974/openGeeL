#ifndef BRDFINTEGRATIONMAP_H
#define BRDFINTEGRATIONMAP_H

#include "texture.h"

namespace geeL {

	//BRDF integration map that stores information for splitsum approximated IBL
	class BRDFIntegrationMap : public Texture {

	public:
		BRDFIntegrationMap();

		virtual unsigned int getID() const;
		virtual void remove();

	private:
		unsigned int id;

	};
}

#endif

