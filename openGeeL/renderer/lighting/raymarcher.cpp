#include "../utility/vectorextension.h"
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "raymarcher.h"

namespace geeL {

	RayMarcher::RayMarcher(RenderScene& scene) 
		: SceneRender(scene, "renderer/lighting/raymarch.frag") {}


	void RayMarcher::bindValues() {
		Transform& transform = camera->transform;

		shader.set<glm::vec3>("camera.position", -transform.getPosition());
		shader.set<glm::vec3>("camera.forward", -transform.getForwardDirection());
		shader.set<glm::vec3>("camera.up", -transform.getUpDirection());
		shader.set<glm::vec3>("camera.right", -transform.getRightDirection());
	}
}