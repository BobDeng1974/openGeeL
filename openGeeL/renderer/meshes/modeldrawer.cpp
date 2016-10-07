#include "modeldrawer.h"
#include "mesh.h"
#include "meshrenderer.h"
#include "model.h"
#include "meshfactory.h"
#include "../shader/shader.h"

#include <list>


namespace geeL {

	MeshDrawer::MeshDrawer(MeshFactory& factory) : factory(factory) {}


	void MeshDrawer::draw() const {

		//Draw the regular mesh renderers
		for (std::list<MeshRenderer>::iterator it = factory.rendererBegin(); it != factory.rendererEnd(); it++)
			it->draw();

		//Draw the instanced mesh renderer. Here the models will be drawn once and 
		//then the individual transformations and materials of the mesh renderers
		for (std::map<Model*, list<MeshRenderer>>::iterator it = factory.instancedModelsBegin();
			it != factory.instancedModelsEnd(); it++) {

			Model& model = *it->first;
			model.drawInstanced();

			for (std::list<MeshRenderer>::iterator iit = it->second.begin(); iit != it->second.end(); iit++)
				iit->draw();
		}
	}

	void MeshDrawer::draw(const Shader& shader) const {
		shader.use();

		for (std::list<MeshRenderer>::iterator it = factory.rendererBegin(); it != factory.rendererEnd(); it++) {

			it->draw(false);

		}
			
	}

}