#include "../shader/shader.h"
#include "drawdefault.h"

namespace geeL {

	DefaultPostProcess::DefaultPostProcess() : PostProcessingEffect("renderer/shaders/screen.frag") {}

	void DefaultPostProcess::draw() {
		shader.use();
	}
}