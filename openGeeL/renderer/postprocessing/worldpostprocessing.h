#ifndef WORLDPOSTPROCESSINGEFFECT_H
#define WORLDPOSTPROCESSINGEFFECT_H

#include <vec3.hpp>
#include <mat4x4.hpp>
#include <list>
#include <string>
#include "../utility/worldrequester.h"
#include "postprocessing.h"

namespace geeL {

	//Special type of post processing effect that uses additional world information (like positions or normals)
	class WorldPostProcessingEffect : public PostProcessingEffect, public WorldInformationRequester {

	public:
		WorldPostProcessingEffect(std::string fragmentPath) 
			: PostProcessingEffect(fragmentPath) {}

		WorldPostProcessingEffect(std::string vertexPath, std::string fragmentPath)
			: PostProcessingEffect(vertexPath, fragmentPath) {}

	};
}

#endif