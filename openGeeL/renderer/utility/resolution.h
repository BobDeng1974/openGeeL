#ifndef RENDERRESOLUTION_H
#define RENDERRESOLUTION_H

namespace geeL {

	//Defines a rendering resolution in normalized coordinates (Between 0 and 1)
	struct Resolution {

	public:
		Resolution(float resolution);

		float get() const;
		void  set(float value);

		template<class T>
		float operator*(T o) const;

	private:
		float resolution;

	};

	const Resolution HALFSCREEN = Resolution(0.5f);
	const Resolution FULLSCREEN = Resolution(1.f);


	inline Resolution::Resolution(float resolution) {
		set(resolution);
	}

	inline float Resolution::get() const {
		return resolution;
	}

	inline void Resolution::set(float value) {
		if (resolution != value && value > 0.f && value <= 1.f)
			resolution = value;
	}

	template<class T>
	inline float Resolution::operator*(T o) const {
		return resolution * o;
	}

	template<class T>
	float operator*(T o, const Resolution& resolution) {
		return resolution * o;
	}


}

#endif
