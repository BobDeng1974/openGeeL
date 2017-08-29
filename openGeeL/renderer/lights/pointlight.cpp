#define GLEW_STATIC
#include <glew.h>
#include <cassert>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "shader/rendershader.h"
#include "shader/sceneshader.h"
#include "transformation/transform.h"
#include "meshes/meshrenderer.h"
#include "cameras/camera.h"
#include "renderscene.h"
#include "pointlight.h"

using namespace std;
using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, const string& name)
		: Light(transform, diffuse, name) {}


	void PointLight::bind(const Shader& shader, const string& name, 
		ShaderTransformSpace space, const Camera* const camera) const {
		
		Light::bind(shader, name, space, camera);

		switch (space) {
			case ShaderTransformSpace::View:
				assert(camera != nullptr);

				shader.bind<glm::vec3>(name + "position", camera->TranslateToViewSpace(transform.getPosition()));
				break;
			case ShaderTransformSpace::World:
				shader.bind<glm::vec3>(name + "position", transform.getPosition());
				break;
		}
		
	}

	
}