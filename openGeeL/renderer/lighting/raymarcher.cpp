#include "utility/vectorextension.h"
#include "transformation/transform.h"
#include "cameras/camera.h"
#include "raymarcher.h"

namespace geeL {

	RayMarcher::RayMarcher(RenderScene& scene) 
		: SceneRender(scene), PostProcessingEffectFS("renderer/lighting/raymarch.frag") {}

	void RayMarcher::init(ScreenQuad& screen, DynamicBuffer& buffer, const Resolution& resolution) {
		PostProcessingEffectFS::init(screen, buffer, resolution);
	}

	void RayMarcher::draw() {
		PostProcessingEffectFS::draw();
	}


	void RayMarcher::bindValues() {
		Transform& transform = camera->transform;

		shader.bind<glm::vec3>("camera.position", -transform.getPosition());
		shader.bind<glm::vec3>("camera.forward", -transform.getForwardDirection());
		shader.bind<glm::vec3>("camera.up", -transform.getUpDirection());
		shader.bind<glm::vec3>("camera.right", -transform.getRightDirection());
	}
}