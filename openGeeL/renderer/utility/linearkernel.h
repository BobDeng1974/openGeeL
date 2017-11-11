#ifndef LINEARKERNEL_H
#define LINEARKERNEL_H

#include <vector>

namespace geeL {

	class Shader;

	//Sparse gaussian kernel that utilizes linear 
	//filtering of GPU to reduce size in ~half
	struct LinearKernel {

	public:
		std::vector<float> weights;
		std::vector<float> offsets;

		void convert(const std::vector<float>& kernel);
		void bind(const Shader& shader, const std::string& weightsName = "weights", 
			const std::string& offsetsName = "offsets") const;

	};
}

#endif
