#ifndef LINEARKERNEL_H
#define LINEARKERNEL_H

#include <vector>

namespace geeL {

	//Sparse gaussian kernel that utilizes linear 
	//filtering of GPU to reduce size in ~half
	struct LinearKernel {

	public:
		std::vector<float> weights;
		std::vector<float> offsets;

		void convert(std::vector<float>& kernel) {
			if (kernel.size() % 2 == 0)
				throw "Size of kernel has to be odd \n";

			size_t newSize = (kernel.size() + 1) / 2;

			weights.clear();
			offsets.clear();
			weights = std::vector<float>(newSize);
			offsets = std::vector<float>(newSize);

			weights[0] = kernel[0];
			offsets[0] = 0.f;

			for (int i = 1; i < kernel.size(); i++) {
				int offset1 = 2 * i - 1; //Index in old structure
				int offset2 = offset1 + 1;

				weights[i] = kernel[offset1] + kernel[offset2];
				offsets[i] = (float(offset1) * kernel[offset1] + float(offset2) * kernel[offset2]) / weights[i];
			}
		}

	};
}

#endif
