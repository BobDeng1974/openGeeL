#ifndef  DOF_H
#define  DOF_H

#include "../utility/framebuffer.h"
#include "worldpostprocessing.h"

namespace geeL {

class GaussianBlur;
class ScreenQuad;

//Depth of field post effect that uses image blurring to achieve effect.
//Cheap but not realistic since circe of confusion is neglected
class DepthOfFieldBlurred : public WorldPostProcessingEffect {

public:
	DepthOfFieldBlurred(GaussianBlur& blur, 
		float focalLength = 5.f, float aperture = 10.f, float farDistance = 100.f, float blurResolution = 1.f);

	~DepthOfFieldBlurred();

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
	float farDistance;
	float blurResolution;

	GaussianBlur& blur;
	FrameBuffer blurBuffer;
	ScreenQuad* blurScreen;

};

}

#endif
