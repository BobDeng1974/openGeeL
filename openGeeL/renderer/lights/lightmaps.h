#ifndef LIGHTMAPS_H
#define LIGHTMAPS_H

#include <functional>
#include <vector>

namespace geeL {

	class ITexture;


	enum class LightMapType {
		None = 0,
		Shadow2D = 1,
		ShadowCube = 2,
		Cookie = 4
	};


	class LightMapContainer {

	public:
		LightMapContainer(unsigned int size = 0);

		void add(const ITexture& texture, LightMapType type);
		void iter(std::function<void(const ITexture&, LightMapType)> function);

		size_t size() const;

	private:
		std::vector<std::pair<const ITexture*, LightMapType>> elements;

	};


	inline LightMapContainer::LightMapContainer(unsigned int size) {
		if(size > 0) elements.reserve(size);
	}

	inline void LightMapContainer::add(const ITexture& texture, LightMapType type) {
		elements.push_back(std::pair<const ITexture*, LightMapType>(&texture, type));
	}

	inline void LightMapContainer::iter(std::function<void(const ITexture&, LightMapType)> function) {
		for (auto it(elements.begin()); it != elements.end(); it++) {
			auto& pair = *it;

			function(*pair.first, pair.second);
		}
	}

	inline size_t LightMapContainer::size() const {
		return elements.size();
	}



	inline LightMapType concat(LightMapType a, LightMapType b) {
		return (LightMapType)((int)a | (int)b);
	}

	inline bool contains(LightMapType container, LightMapType contemplate) {
		LightMapType c = (LightMapType)((int)container & (int)contemplate);
		return c == contemplate;
	}


}

#endif
