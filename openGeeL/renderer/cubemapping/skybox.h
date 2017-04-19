#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <mat3x3.hpp>
#include <mat4x4.hpp>
#include "../shader/shader.h"

using glm::mat3;
using glm::mat4;

namespace geeL {

	class SceneCamera;
	class CubeMap;
	class IrradianceMap;
	class Shader;

	//Object that describes the static environment of the scene
	class Skybox {

	public:
		Skybox(const CubeMap& cubeMap);
		Skybox(const IrradianceMap& irradianceMap);

		void draw(const SceneCamera& camera) const;
		void bind(Shader& shader) const;

		unsigned int getID() const;

	private:
		const CubeMap& cubeMap;
		Shader shader;

	};
}

#endif 