#include "simpleblur.h"

namespace geeL {

	SimpleBlur::SimpleBlur(unsigned int strength)
		: PostProcessingEffectFS("renderer/postprocessing/simpleblur.frag"), amount(strength + 1) {}

	SimpleBlur::SimpleBlur(unsigned int strength, std::string shaderPath)
		: PostProcessingEffectFS(shaderPath), amount(strength + 1) {}


	void SimpleBlur::init(const PostProcessingParameter& parameter) {
		PostProcessingEffectFS::init(parameter);

		shader.bind<int>("amount", amount);
	}


	SimpleBlur3D::SimpleBlur3D(unsigned int strength) 
		: SimpleBlur(strength, "renderer/postprocessing/simpleblur3d.frag") {}

}