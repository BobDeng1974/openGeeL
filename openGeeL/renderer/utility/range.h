#ifndef RANGE_H
#define RANGE_H

#include <limits>

namespace geeL {

	template<class T>
	struct Range {

	public:
		Range(T min, T max) : min(min), max(max) {}

		bool inside(T value) {
			return min < value && value <= max;
		}

		T clamp(T value) {
			return (value < min) ? min : (value > max) ? max : value;
		}

		const T& getMin() const {
			return min;
		}

		const T& getMax() const {
			return max;
		}

	private:
		T min, max;

	};

	const Range<float> UNLIMITED_FLOAT_RANGE(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
	const Range<float> POSITIVE_FLOAT_RANGE(0.f, std::numeric_limits<float>::max());
	const Range<float> NORMALIZED_FLOAT_RANGE(0.f, 1.f);

	const Range<int> UNLIMITED_INT_RANGE(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
	const Range<int> POSITIVE_INT_RANGE(0, std::numeric_limits<int>::max());
	const Range<int> NORMALIZED_INT_RANGE(0, 1);

}

#endif
