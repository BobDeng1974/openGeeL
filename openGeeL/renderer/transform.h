#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm.hpp>
#include <vector>

using namespace glm;

namespace geeL {

class Transform {

public:
	Transform* parent = nullptr;
	bool isStatic;

	vec3 position;
	vec3 rotation;
	vec3 scaling;
	vec3 forward;
	vec3 up;
	vec3 right;

	mat4 matrix;

	Transform();
	Transform(vec3 position, vec3 rotation, vec3 scaling);
	~Transform();
		
	void translate(vec3 translation);
	void rotate(vec3 axis, float angle);
	void scale(vec3 scalar);

	mat4 lookAt() const;

	std::vector<Transform*>::iterator childrenStart();
	std::vector<Transform*>::iterator childrenEnd();

	void computeMatrix();

private:
	std::vector<Transform*> children;

	void updateDirections();

};


}

#endif
