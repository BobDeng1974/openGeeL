#include "defaults.h"

namespace geeL {

	const float DEF_FLOAT(0.f);
	const int DEF_INTEGER(0);
	const unsigned int DEF_UINTEGER(0U);

	const glm::vec2 DEF_VEC2(0.f);
	const glm::vec3 DEF_VEC3(0.f);
	const glm::vec4 DEF_VEC4(0.f);
	const glm::mat3 DEF_MAT3(0.f);
	const glm::mat4 DEF_MAT4(0.f);

	const gvec2 DEF_GVEC2(0.f);
	const gvec3 DEF_GVEC3(0.f);
	const gvec4 DEF_GVEC4(0.f);
	const gmat3 DEF_GMAT3(0.f);
	const gmat4 DEF_GMAT4(0.f);


	const float& DefaultValues::get(float i) {
		return DEF_FLOAT;
	}

	const int& DefaultValues::get(int i) {
		return DEF_INTEGER;
	}

	const unsigned int& DefaultValues::get(unsigned int i) {
		return DEF_UINTEGER;
	}

	const glm::vec2& DefaultValues::get(const glm::vec2& i) {
		return DEF_VEC2;
	}

	const glm::vec3& DefaultValues::get(const glm::vec3& i) {
		return DEF_VEC3;
	}

	const glm::vec4& DefaultValues::get(const glm::vec4& i) {
		return DEF_VEC4;
	}

	const glm::mat3& DefaultValues::get(const glm::mat3& i) {
		return DEF_MAT3;
	}

	const glm::mat4& DefaultValues::get(const glm::mat4& i) {
		return DEF_MAT4;
	}

	const gvec2& DefaultValues::get(const gvec2& i) {
		return DEF_GVEC2;
	}

	const gvec3& DefaultValues::get(const gvec3& i) {
		return DEF_GVEC3;
	}

	const gvec4& DefaultValues::get(const gvec4& i) {
		return DEF_GVEC4;
	}

	const gmat3& DefaultValues::get(const gmat3& i) {
		return DEF_GMAT3;
	}

	const gmat4& DefaultValues::get(const gmat4& i) {
		return DEF_GMAT4;
	}

}