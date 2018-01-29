#include <cassert>
#include <cmath>
#include <functional>
#include "instancedmesh.h"
#include "meshrenderer.h"
#include "meshidgenerator.h"

using namespace std;

namespace geeL{

	unsigned int MeshRendererIDGenerator::bucketCounters[4] = { 0, 0, 0, 0 };
	unsigned int MeshRendererIDGenerator::bucketSizes[4] = { 2, 5, 8, 11 };

	std::map<const MeshRenderer*, unsigned short> MeshRendererIDGenerator::localMeshIDs;
	std::queue<unsigned short> MeshRendererIDGenerator::a;
	std::queue<unsigned short> MeshRendererIDGenerator::b;
	std::queue<unsigned short> MeshRendererIDGenerator::c;
	std::queue<unsigned short> MeshRendererIDGenerator::d;
	
	std::queue<unsigned short>* MeshRendererIDGenerator::freeIDs[4] = {
		&MeshRendererIDGenerator::a, 
		&MeshRendererIDGenerator::b, 
		&MeshRendererIDGenerator::c, 
		&MeshRendererIDGenerator::d };


	unsigned short MeshRendererIDGenerator::generateID(MeshRenderer& renderer, size_t meshCount) {
		renderer.addDeleteListener([](const MeshRenderer& renderer) {
			removeMeshRenderer(renderer);
		});

		localMeshIDs[&renderer] = 0;

		return generateID(meshCount);
	}

	unsigned short MeshRendererIDGenerator::generateID(size_t meshCount) {
		assert(meshCount < 2048 && "Too many meshes attached");

		long double mc = static_cast<long double>(meshCount - 1);
		mc = log2(mc);
		mc = ceil(mc);

		unsigned int requiredBits = static_cast<unsigned int>(mc);

		unsigned int bucketNumber = 0;
		for (unsigned int i = 0; i < 4; i++) {
			if (requiredBits <= bucketSizes[i]) {
				bucketNumber = i;
				break;
			}
		}

		std::queue<unsigned short>& q = *freeIDs[bucketNumber];

		//Use existing ID that has been freed from a deleted mesh renderer
		if (!q.empty()) {
			unsigned short id = q.front();
			q.pop();

			return id;
		}
		//Otherwise generate a new ID
		else {
			unsigned int bucketMax = pow(2, 14 - bucketSizes[bucketNumber]);
			assert(bucketCounters[bucketNumber] < bucketMax && "ID capacity exhausted for given mesh count");

			unsigned short id = bucketNumber * 16384;
			id += bucketCounters[bucketNumber]++ * pow(2, bucketSizes[bucketNumber]);

			return id;
		}
	}

	unsigned short MeshRendererIDGenerator::generateMeshID(const MeshRenderer& renderer) {
		auto it = localMeshIDs.find(&renderer);
		assert(it != localMeshIDs.end() && "Given mesh renderer has no ID yet");

		unsigned short& idCounter = it->second;
		unsigned short meshID = renderer.getID() + idCounter++;

		return meshID;
	}

	short int indicies[4] = { 3, 2, 1, 0 };
	unsigned short MeshRendererIDGenerator::getID(unsigned short globalMeshID) {
		unsigned short prefix = globalMeshID >> 14;
		unsigned short offset = (indicies[prefix] + 1) * 3 + 2;
		unsigned short id = (globalMeshID >> offset) << offset;

		return id;
	}

	unsigned short MeshRendererIDGenerator::getID(MeshInstance& mesh) {
		return getID(mesh.getID());
	}


	void MeshRendererIDGenerator::removeMeshRenderer(const MeshRenderer& renderer) {
		unsigned short id = renderer.getID();
		unsigned short prefix = id >> 14;

		std::queue<unsigned short>& q = *freeIDs[prefix];
		q.push(id);

		localMeshIDs.erase(&renderer);
	}

}