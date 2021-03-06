#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <mat3x3.hpp>
#include <mat4x4.hpp>
#include "shader/rendershader.h"

using glm::mat3;
using glm::mat4;

namespace geeL {

	class Camera;
	class CubeMap;
	class IrradianceMap;
	class RenderShader;

	//Object that describes the static environment of the scene
	class Skybox {

	public:
		Skybox(const CubeMap& cubeMap, float brightnessScale = 1.f);

		void draw(const Camera& camera) const;
		void bind(RenderShader& shader) const;

		float getBrightnessScale() const;
		void  setBrightnessScale(float value);

	private:
		float brightnessScale;
		const CubeMap& cubeMap;
		RenderShader shader;

	};
}

#endif 