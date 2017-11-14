#include <vector>
#include "resolution.h"

namespace geeL {

	std::vector<ResolutionPreset> scales = {
		ResolutionPreset::TEN, 
		ResolutionPreset::TWENTY,
		ResolutionPreset::TWENTYFIVE, 
		ResolutionPreset::ONETHIRD, 
		ResolutionPreset::FORTY,
		ResolutionPreset::HALFSCREEN,
		ResolutionPreset::TWOTHIRDS,
		ResolutionPreset::SEVENTYFIVE,
		ResolutionPreset::EIGHTY,
		ResolutionPreset::NINETY,
		ResolutionPreset::FULLSCREEN 
	};



	ResolutionScale getResolutionScale(ResolutionPreset resolution) {

		switch (resolution) {
			case ResolutionPreset::TWOTHIRDS:
				return ResolutionScale(2.f / 3.f);
			case ResolutionPreset::ONETHIRD:
				return ResolutionScale(1.f / 3.f);
		}

		return ResolutionScale((float)resolution / 100.f);
	}

	size_t getRenderResolutionCount() {
		return scales.size();
	}

	size_t getRenderResolutionIndex(ResolutionPreset resolution) {
		for (int i = 0; i < scales.size(); i++) {
			if (resolution == scales[i])
				return i;
		}

		return scales.size() - 1;
	}

	ResolutionPreset getRenderResolution(size_t index) {
		if (index >= scales.size())
			return ResolutionPreset::FULLSCREEN;

		return scales[index];
	}

}