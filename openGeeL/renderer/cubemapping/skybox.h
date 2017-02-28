#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <mat3x3.hpp>
#include <mat4x4.hpp>

using glm::mat3;
using glm::mat4;

namespace geeL {

	class Camera;
	class CubeMap;
	class Shader;

	class Skybox {

	public:
		const CubeMap& cubeMap;

		Skybox(CubeMap& cubeMap);

		void draw(const Camera& camera) const;
		void bind(const Shader& shader) const;

		unsigned int getID() const;

	private:
		Shader shader;

	};
}

#endif 