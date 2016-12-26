#ifndef SSRR_H
#define SSRR_H

#include "worldpostprocessing.h"

namespace geeL {

class Camera;

//Screen Space Raycasted Reflections post effect
class SSRR : public WorldPostProcessingEffect {

public:
	SSRR(const Camera& camera);

	virtual WorldMaps requiredWorldMaps() const;
	virtual WorldMatrices requiredWorldMatrices() const;
	virtual WorldVectors requiredWorldVectors() const;
	virtual std::list<WorldMaps> requiredWorldMapsList() const;

	virtual void addWorldInformation(std::list<unsigned int> maps,
		std::list<glm::mat4> matrices, std::list<glm::vec3> vectors);

protected:
	virtual void bindValues();

private:
	const Camera& camera;


};

}


#endif

