#include <vector>
#include "resolution.h"

namespace geeL {

	std::vector<RenderResolution> scales = {
		RenderResolution::TEN, 
		RenderResolution::TWENTY,
		RenderResolution::TWENTYFIVE, 
		RenderResolution::ONETHIRD, 
		RenderResolution::FORTY,
		RenderResolution::HALFSCREEN,
		RenderResolution::TWOTHIRDS,
		RenderResolution::SEVENTYFIVE,
		RenderResolution::EIGHTY,
		RenderResolution::NINETY,
		RenderResolution::FULLSCREEN 
	};



	ResolutionScale getResolutionScale(RenderResolution resolution) {

		switch (resolution) {
			case RenderResolution::TWOTHIRDS:
				return ResolutionScale(2.f / 3.f);
			case RenderResolution::ONETHIRD:
				return ResolutionScale(1.f / 3.f);
		}

		return ResolutionScale((float)resolution / 100.f);
	}

	size_t getRenderResolutionCount() {
		return scales.size();
	}

	size_t getRenderResolutionIndex(RenderResolution resolution) {
		for (int i = 0; i < scales.size(); i++) {
			if (resolution == scales[i])
				return i;
		}

		return scales.size() - 1;
	}

	RenderResolution getRenderResolution(size_t index) {
		if (index >= scales.size())
			return RenderResolution::FULLSCREEN;

		return scales[index];
	}


}