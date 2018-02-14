#ifndef MESHIDGENERATOR_H
#define MESHIDGENERATOR_H

#include <map>
#include <queue>

namespace geeL {

	class SingleMeshRenderer;


	class MeshRendererIDGenerator {

	public:
		//Generate a new ID for given mesh renderer
		static unsigned short generateID(SingleMeshRenderer& renderer, size_t meshCount);
		
		//Generate an new ID for a mesh instance of given mesh renderer
		//Note: Mesh renderer must already have an ID generated
		static unsigned short generateMeshID(const SingleMeshRenderer& renderer);

		//Reconstruct ID for mesh renderer that contains a mesh with given id
		static unsigned short getID(unsigned short meshID);

	private:
		static unsigned int bucketCounters[4];
		static unsigned int bucketSizes[4];

		static std::map<const SingleMeshRenderer*, unsigned short> localMeshIDs;
		static std::queue<unsigned short> a, b, c, d;
		static std::queue<unsigned short>* freeIDs[4];


		static unsigned short generateID(size_t meshCount);
		static void removeMeshRenderer(const SingleMeshRenderer& renderer);

	};

}

#endif
