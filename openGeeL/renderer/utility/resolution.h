#ifndef RENDERRESOLUTION_H
#define RENDERRESOLUTION_H

#include <string>
#include "viewport.h"

namespace geeL {

	//Defines a rendering resolution scale in normalized coordinates (Between 0 and 1)
	struct ResolutionScale {

	public:
		ResolutionScale(float scale);

		float get() const;
		void  set(float value);

		template<class T>
		float operator*(T o) const;

	private:
		float scale;

	};


	//Defines a rendering resolution in absolute pixel size
	struct Resolution {

	public:
		Resolution();
		Resolution(unsigned int resolution);
		Resolution(unsigned int width, unsigned int height);
		Resolution(const Resolution& resolution, const ResolutionScale& scale);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		void resize(ResolutionScale& scale);
		void setRenderResolution() const;

		template<class T>
		float operator*(T o) const;

		bool operator== (const Resolution& o) const;
		bool operator!= (const Resolution& o) const;

		std::string toString() const;

	private:
		unsigned int width, height, baseWidth, baseHeight;

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

	inline void Resolution::setRenderResolution() const {
		Viewport::set(0, 0, width, height);
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


	const ResolutionScale HALFSCREEN = ResolutionScale(0.5f);
	const ResolutionScale FULLSCREEN = ResolutionScale(1.f);


	inline ResolutionScale::ResolutionScale(float scale) {
		set(scale);
	}

	inline float ResolutionScale::get() const {
		return scale;
	}

	inline void ResolutionScale::set(float value) {
		if (scale != value && value > 0.f && value <= 1.f)
			scale = value;
	}

	template<class T>
	inline float ResolutionScale::operator*(T o) const {
		return scale * o;
	}

	template<class T>
	float operator*(T o, const ResolutionScale& scale) {
		return scale * o;
	}

	inline Resolution::Resolution(const Resolution& resolution, const ResolutionScale& scale)
		: width(unsigned int(resolution.getWidth() * scale)), height(unsigned int(resolution.getHeight() * scale)), 
			baseWidth(resolution.getWidth()), baseHeight(resolution.getHeight()) {}

	inline void Resolution::resize(ResolutionScale& scale) {
		width = unsigned int(baseWidth * scale);
		height = unsigned int(baseHeight * scale);
	}

}

#endif
