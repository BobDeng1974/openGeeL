#ifndef RENDERRESOLUTION_H
#define RENDERRESOLUTION_H

#include <string>
#include <vec2.hpp>
#include "glwrapper/viewport.h"

namespace geeL {

	struct ResolutionScale;


	enum class ResolutionPreset {
		FULLSCREEN = 100,
		NINETY = 90,
		EIGHTY = 80,
		SEVENTYFIVE = 75,
		TWOTHIRDS = 66,
		HALFSCREEN = 50,
		FORTY = 40,
		ONETHIRD = 33,
		TWENTYFIVE = 25,
		TWENTY = 20,
		TEN = 10
	};

	ResolutionScale getResolutionScale(ResolutionPreset resolution);
	ResolutionPreset getRenderPreset(size_t index);
	ResolutionPreset getRenderPreset(ResolutionScale resolution);
	size_t getRenderResolutionCount();
	size_t getRenderResolutionIndex(ResolutionPreset resolution);
	


	//Defines a rendering resolution in absolute pixel size
	struct Resolution {

	public:
		Resolution();
		Resolution(unsigned int resolution);
		Resolution(unsigned int width, unsigned int height);
		Resolution(const Resolution& resolution, const ResolutionScale& scale);
		Resolution(const Resolution& resolution, const ResolutionPreset& scale);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		ResolutionScale getScale() const;

		void resize(ResolutionScale& scale);
		void applyRenderResolution() const;

		template<class T>
		float operator*(T o) const;

		bool operator== (const Resolution& o) const;
		bool operator!= (const Resolution& o) const;

		operator glm::vec2() const;
		std::string toString() const;

	private:
		unsigned int width, height, baseWidth, baseHeight;

	};



	//Defines a rendering resolution scale in normalized coordinates (Between 0 and 1)
	struct ResolutionScale {

	public:
		ResolutionScale(float scale);
		ResolutionScale(ResolutionPreset resolution);

		float get() const;
		void  set(float value);

		template<class T>
		float operator*(T o) const;

		bool operator== (const ResolutionScale& o) const;
		bool operator!= (const ResolutionScale& o) const;

	private:
		float scale;

	};


	


	inline Resolution::Resolution() {}

	inline Resolution::Resolution(unsigned int resolution)
		: width(resolution), height(resolution), baseWidth(resolution), baseHeight(resolution) {}

	inline Resolution::Resolution(unsigned int width, unsigned int height) 
		: width(width), height(height), baseWidth(width), baseHeight(height) {}


	inline unsigned int Resolution::getWidth() const {
		return width;
	}

	inline unsigned int Resolution::getHeight() const {
		return height;
	}

	inline ResolutionScale Resolution::getScale() const {
		return (float)width / (float)baseWidth;
	}

	inline void Resolution::applyRenderResolution() const {
		Viewport::set(0, 0, width, height);
	}

	inline Resolution::operator glm::vec2() const {
		return glm::vec2(width, height);
	}

	inline std::string Resolution::toString() const {
		return "(" + std::to_string(baseWidth) + ", " + std::to_string(baseHeight) + ") -> (" 
			+ std::to_string(width) + ", " + std::to_string(height) + ")";
	}

	template<class T>
	inline float Resolution::operator*(T o) const {
		width = unsigned int(width * o);
		height = unsigned int(height * o);
	}

	template<class T>
	float operator*(T o, const Resolution& resolution) {
		return resolution * o;
	}

	inline bool Resolution::operator== (const Resolution& o) const {
		return (width == o.width) && (height == o.height);
	}

	inline bool Resolution::operator!= (const Resolution& o) const {
		return (width != o.width) || (height != o.height);
	}


	inline ResolutionScale::ResolutionScale(float scale) {
		set(scale);
	}

	inline ResolutionScale::ResolutionScale(ResolutionPreset resolution) {
		set(getResolutionScale(resolution).get());
	}

	inline float ResolutionScale::get() const {
		return scale;
	}

	inline void ResolutionScale::set(float value) {
		if (scale != value)
			scale = (value < 0.f) ? 0.f : (value > 2.f) ? 2.f : value;
	}

	template<class T>
	inline float ResolutionScale::operator*(T o) const {
		return scale * o;
	}

	template<class T>
	inline float operator*(T o, const ResolutionScale& scale) {
		return scale * o;
	}

	inline bool ResolutionScale::operator== (const ResolutionScale& o) const {
		return scale == o.scale;
	}

	inline bool ResolutionScale::operator!= (const ResolutionScale& o) const {
		return scale != o.scale;
	}

	inline bool operator< (const ResolutionScale& lhs, const ResolutionScale& rhs) {
		return lhs.get() < rhs.get();
	}

	inline Resolution::Resolution(const Resolution& resolution, const ResolutionScale& scale)
		: width(unsigned int(resolution.getWidth() * scale)), height(unsigned int(resolution.getHeight() * scale)), 
			baseWidth(resolution.getWidth()), baseHeight(resolution.getHeight()) {}

	inline Resolution::Resolution(const Resolution& resolution, const ResolutionPreset& scale)
		: Resolution(resolution, getResolutionScale(scale)) {}


	inline void Resolution::resize(ResolutionScale& scale) {
		width = unsigned int(baseWidth * scale);
		height = unsigned int(baseHeight * scale);
	}

}

#endif
