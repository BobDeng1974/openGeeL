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
		BRDFIntegrationMap(const BRDFIntegrationMap& map);

		virtual unsigned int getID() const;
		virtual void remove();

		virtual void add(RenderShader& shader, std::string name) const;

	private:
		unsigned int id;

	};
}

#endif

