#ifndef DEFAULTVALUES_H
#define DEFAULTVALUES_H

#include "properties.h"

namespace geeL {


	//Provides default values for common shader parameters
	class DefaultValues {

	public:
		template<class T>
		static const T& get();


	private:
		static const float& get(float i);
		static const int& get(int i);
		static const unsigned int& get(unsigned int i);

		static const glm::vec2& get(const glm::vec2& i);
		static const glm::vec3& get(const glm::vec3& i);
		static const glm::vec4& get(const glm::vec4& i);
		static const glm::mat3& get(const glm::mat3& i);
		static const glm::mat4& get(const glm::mat4& i);

		static const gvec2& get(const gvec2& i);
		static const gvec3& get(const gvec3& i);
		static const gvec4& get(const gvec4& i);
		static const gmat3& get(const gmat3& i);
		static const gmat4& get(const gmat4& i);

	};


	template<class T>
	inline const T& DefaultValues::get() {
		return get(T());
	}

}

#endif
