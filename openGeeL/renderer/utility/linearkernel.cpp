#include <string>
#include "shader/shader.h"
#include "linearkernel.h"

using namespace std;

namespace geeL {
	
	void LinearKernel::convert(const std::vector<float>& kernel) {
		if (kernel.size() % 2 == 0)
			throw "Size of kernel has to be odd\n";

		size_t newSize = (kernel.size() + 1) / 2;
		weights.resize(newSize);
		offsets.resize(newSize);

		weights[0] = kernel[0];
		offsets[0] = 0.f;

		for (size_t i = 1; i < newSize; i++) {
			int offset1 = 2 * i - 1; //Index in old structure
			int offset2 = offset1 + 1;

			weights[i] = kernel[offset1] + kernel[offset2];
			offsets[i] = (float(offset1) * kernel[offset1] + float(offset2) * kernel[offset2]) / weights[i];
		}
	}

	void LinearKernel::bind(const Shader& shader, const string& weightsName, const string& offsetsName) const {
		for (size_t i = 0; i < weights.size(); i++) {
			string index = std::to_string(i) + "]";

			shader.bind<float>(weightsName + "[" + index, weights[i]);
			shader.bind<float>(offsetsName + "[" + index, offsets[i]);
		}
	}

}