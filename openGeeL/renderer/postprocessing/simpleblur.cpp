#include "simpleblur.h"

namespace geeL {

	SimpleBlur::SimpleBlur(unsigned int strength)
		: PostProcessingEffect("renderer/postprocessing/simpleblur.frag"), amount(strength + 1) {}


	void SimpleBlur::bindValues() {
		PostProcessingEffect::bindValues();

		shader.setInteger("amount", amount);
	}
}