#ifndef BRDFINTEGRATIONMAP_H
#define BRDFINTEGRATIONMAP_H

#include <string>
#include "texture.h"

namespace geeL {

	class Shader;

	//BRDF integration map that stores information for splitsum approximated IBL
	class BRDFIntegrationMap : public Texture {

	public:
		BRDFIntegrationMap();
		BRDFIntegrationMap(const BRDFIntegrationMap& map);

		virtual unsigned int getID() const;
		virtual void remove();

		virtual void add(Shader& shader, std::string name) const;

	private:
		unsigned int id;

	};
}

#endif

