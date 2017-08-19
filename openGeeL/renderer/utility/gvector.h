#ifndef GEELVECTOR_H
#define GEELVECTOR_H

#include <vec3.hpp>
#include <geometric.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace geeL {

	template<typename T>
	class gVector {

	public:
		gVector(const T& vector);

		void normalize();
		int length();

		gVector<T>& operator= (const gVector<T>& other);
		gVector<T>& operator= (const T& other);


	private:
		T vector;


	};



	template<typename T>
	inline gVector<T>::gVector(const T& vector) : vector(vector) {}

	template<typename T>
	inline void gVector<T>::normalize() {
		vector = glm::normalize(vector);
	}

	template<typename T>
	inline int gVector<T>::length() {
		return vector.length;
	}

	template<typename T>
	inline gVector<T>& gVector<T>::operator=(const gVector<T>& other) {
		if (&other != this) {
			vector = other.vector;

		}

		return *this;
	}

	template<typename T>
	inline gVector<T>& gVector<T>::operator=(const T& other) {
		vector = other;

		return *this;
	}



}

#endif
