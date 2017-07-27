#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat3x3.hpp>
#include <mat4x4.hpp>
#include "property.h"

namespace geeL {

	using gvec2 = glm::detail::tvec2<geeL::PropertyBase<float>, glm::highp>;
	using gvec3 = glm::detail::tvec3<geeL::PropertyBase<float>, glm::highp>;
	using gvec4 = glm::detail::tvec4<geeL::PropertyBase<float>, glm::highp>;

	using gmat3 = glm::detail::tmat3x3<geeL::PropertyBase<float>, glm::highp>;
	using gmat4 = glm::detail::tmat4x4<geeL::PropertyBase<float>, glm::highp>;

	using gFloat = geeL::Property<float>;
	using gInteger = geeL::Property<int>;
	using gUInteger = geeL::Property<unsigned int>;

	using gVector2 = geeL::NestedProperty1x<gvec2, float>;
	using gVector3 = geeL::NestedProperty1x<gvec3, float>;
	using gVector4 = geeL::NestedProperty1x<gvec4, float>;

	using gMat3 = geeL::NestedProperty2x<gmat3, gvec3, float>;
	using gMat4 = geeL::NestedProperty2x<gmat4, gvec4, float>;


	class Properties {

	public:
		//Converts a g-vector to a glm vector
		template<class T, class V>
		static T convertVec(const V& gval);

		//Converts a g-matrix to a glm matrix
		template<class T, class V>
		static T convertMat(const V& gval);

	};


	template<class T, class V>
	inline T Properties::convertVec(const V & gval) {
		T vec;

		for (int i = 0; i < gval.length(); i++)
			vec[i] = gval[i];

		return vec;
	}

	template<class T, class V>
	inline T Properties::convertMat(const V& gval) {
		T mat;

		for (int i = 0; i < gval.length(); i++) {
			for (int j = 0; j < gval.length(); j++)
				mat[i][j] = gval[i][j];
		}

		return mat;
	}

}

#endif
