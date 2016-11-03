#ifndef SSAO_H
#define SSAO_H

#include <vector>
#include <vec3.hpp>
#include "../texturing/simpletexture.h"
#include "../utility/framebuffer.h"
#include "postprocessing.h"

namespace geeL {

class Camera;
class SimpleBlur;

class SSAO : public PostProcessingEffect {

public:
	SSAO(const Camera& camera, SimpleBlur& blur, float radius = 1.f);

	virtual void setScreen(ScreenQuad& screen);
	virtual void draw();

protected:
	virtual void bindValues();

private:
	bool ssaoPass = false;
	const float radius;
	unsigned int sampleCount = 64;
	SimpleTexture noiseTexture;
	std::vector<glm::vec3> kernel;
	std::vector<glm::vec3> noise;
	const Camera& camera;
	SimpleBlur& blur;
	FrameBuffer tempBuffer;
	

};

}

#endif