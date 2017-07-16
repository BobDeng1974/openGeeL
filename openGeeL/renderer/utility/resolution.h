#ifndef RENDERRESOLUTION_H
#define RENDERRESOLUTION_H

namespace geeL {

	//Defines a rendering resolution in absolute pixel size
	struct Resolution {

	public:
		Resolution(unsigned int width, unsigned int height);

		unsigned int getWidth() const;
		unsigned int getHeight() const;

		template<class T>
		float operator*(T o) const;

	private:
		unsigned int width, height;

	};



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


	inline Resolution::Resolution(unsigned int width, unsigned int height) 
		: width(width), height(height) {}

	inline unsigned int Resolution::getWidth() const {
		return width;
	}

	inline unsigned int Resolution::getHeight() const {
		return height;
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


}

#endif
