#include <iostream>
#include "../transformation/transform.h"
#include "../cameras/camera.h"
#include "raymarcher.h"

namespace geeL {

	RayMarcher::RayMarcher(RenderScene& scene) 
		: SceneRender(scene, "renderer/postprocessing/raymarch.frag") {}


	void RayMarcher::bindValues() {
		Transform& transform = camera->transform;

		shader.setVector3("camera.position", transform.getPosition());
		shader.setVector3("camera.forward", transform.getForwardDirection());
		shader.setVector3("camera.up", transform.getUpDirection());
		shader.setVector3("camera.right", transform.getRightDirection());
	}
}