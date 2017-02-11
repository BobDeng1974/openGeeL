#ifndef  DOF_H
#define  DOF_H

#include "../utility/framebuffer.h"
#include "worldpostprocessing.h"

namespace geeL {

class GaussianBlur;

//Depth of field post effect that uses image blurring to achieve effect.
//Cheap but not realistic since circe of confusion is neglected
class DepthOfFieldBlurred : public WorldPostProcessingEffect {

public:
	DepthOfFieldBlurred(GaussianBlur& blur, float focalLength = 5.f, float aperture = 2.f);

	virtual void setScreen(ScreenQuad& screen);

	virtual WorldMaps requiredWorldMaps() const;
	virtual WorldMatrices requiredWorldMatrices() const;
	virtual WorldVectors requiredWorldVectors() const;
	virtual std::list<WorldMaps> requiredWorldMapsList() const;

	virtual void addWorldInformation(std::list<unsigned int> maps,
		std::list<glm::mat4> matrices, std::list<glm::vec3> vectors);

protected:
	virtual void bindValues();

private:
	float focalLength;
	float aperture;

	GaussianBlur& blur;
	FrameBuffer blurBuffer;

};

}

#endif
