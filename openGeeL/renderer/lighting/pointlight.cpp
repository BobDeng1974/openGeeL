#define GLEW_STATIC
#include <glew.h>
#include <gtc/matrix_transform.hpp>
#include <string>
#include "../shader/shader.h"
#include "../shader/sceneshader.h"
#include "../transformation/transform.h"
#include "../meshes/meshrenderer.h"
#include "../cameras/camera.h"
#include "../scene.h"
#include "pointlight.h"

using namespace std;
using namespace glm;

namespace geeL {

	PointLight::PointLight(Transform& transform, vec3 diffuse, const string& name)
		: Light(transform, diffuse, name) {}


	void PointLight::bind(const RenderScene& scene, const Shader& shader, 
		const string& name, ShaderTransformSpace space) const {
		Light::bind(scene, shader, name, space);

		switch (space) {
			case ShaderTransformSpace::View:
				shader.setVector3(name + "position", scene.TranslateToViewSpace(transform.getPosition()));
				break;
			case ShaderTransformSpace::World:
				shader.setVector3(name + "position", transform.getPosition());
				break;
		}
		
	}

}